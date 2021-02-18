//
// Created by ingebrigt on 17.02.2021.
// Edited by Erling on 17.02.2021.
//

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <list>
#include <mutex>
#include "condition_variable"

static bool cont = true;
class Workers {

    std::condition_variable cv;
    std::list<std::function<void()>> tasks;
    std::mutex task_lock;
    std::vector<std::thread> threads;
    int numThreads;

public:
    Workers(int numThreads) {
        this->numThreads = numThreads;
    }

    void post(std::function<void()> *f) {
        std::cout << "post start" << std::endl;
        //guarantees that the lock will be unlocked when leaving function scope
        std::unique_lock<std::mutex> guard(task_lock);
        tasks.emplace_back(*f);
        guard.unlock();
        //notify one thread as one possible task has been added
        cv.notify_one();
        std::cout << "post end" << std::endl;
    }

    void start() {
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back( std::thread{&Workers::consume, this}
            );
        }
    }

    void consume() {
        //want to run forever until join() is called
        while (cont) {
            //references same mutex so will be locked at same time as mutex in post()
            //unlocked when the thread later wakes up
            std::unique_lock<std::mutex> guard(task_lock);
            cv.wait(guard);
            std::cout << "thread wakes" << std::endl;
            if (!tasks.empty()) {
                std::cout << "consume, list of tasks not empty" << std::endl;

                std::function<void()> &f = tasks.front();
                tasks.pop_front();
                guard.unlock();

                f();
                std::cout << "task finished" << std::endl;
            }
        }
    }

    //denne er vel bare det samme som join()??
    void stop() {

    }

    void post_timeout(std::function<void()> func, int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        func();
    }

    void join() {
        //ensure that no more tasks will be started, then finish the while-loop and make every thread wake up
        cont = false;
        cv.notify_all();
        for (int i = 0; i < numThreads; ++i) {
            std::cout << "attempting to join thread nr: " << i << std::endl;
            if(threads[i].joinable()){
                threads[i].join();
                std::cout << "thread nr " << i << " joined" << std::endl;
            }
        }
    }
};
