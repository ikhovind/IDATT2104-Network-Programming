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
#include <deque>
#include "condition_variable"

class Workers {

    std::condition_variable cv;
    std::list<std::function<void()>> tasks;
    std::mutex task_lock;
    std::vector<std::thread> threads;
    int numThreads;

public:
    Workers(int numThreads) {
        threads = std::vector<std::thread>(numThreads);
        this->numThreads = numThreads;
    }

    //ta inn funksjoner som parameter pog
    void post(std::function<void()> *f) {
        //TODO does not enter this at all due to infinite loop in consume()
        //guarantees that the lock will be unlocked when leaving scope
        std::cout << "poop" << std::endl;
        std::unique_lock<std::mutex> guard(task_lock);
        tasks.emplace_back(*f);
        guard.unlock();
        //notify one thread as one possible task has been added
        cv.notify_one();
        std::cout << "poop2" << std::endl;
    }

    void start() {
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(
                    std::thread(&Workers::consume, this)
            );
        }
    }

    void consume() {
        //want to run forever until join() is called
        while (true) {
            //references same mutex so will be locked at same time as mutex in post()
            //unlocked when
            std::unique_lock<std::mutex> guard(task_lock);
            //TODO doesnt sleep at all runs loop continously
            cv.wait(guard);
            if (!tasks.empty()) {
                std::function<void()> f = tasks.front();
                std::cout << "fittetryne" << std::endl;
                std::cout << tasks.empty() << std::endl;

                f();
                tasks.pop_front();

                guard.unlock();
                std::cout << "troop" << std::endl;
            }
        }
        //kjør venting eller noe slikt som han viste i timen mens man venter på at post() kjører??
    }

    //denne er vel bare det samme som join()??
    void stop() {

    }

    void post_timeout(std::function<void()> func, int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        func();
    }

    void join() {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        for (int i = 0; i < numThreads; ++i) {
            if(threads[i].joinable()){
                threads.at(i).join();
            }
        }
    }
};
