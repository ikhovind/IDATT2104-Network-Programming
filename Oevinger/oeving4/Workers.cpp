#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <list>
#include <mutex>
#include "condition_variable"

class Workers {
    bool cont = true;
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
        //unique guarantees that the lock will be unlocked when leaving function scope
        std::unique_lock<std::mutex> guard(task_lock);
        tasks.emplace_back(*f);
        guard.unlock();
        //notify one thread as one possible task has been added
        cv.notify_one();
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
            std::unique_lock<std::mutex> guard(task_lock);
            cv.wait(guard, [this]{return !tasks.empty() || !cont;});
            if(!cont) return;
            //deep copy as pop_front destroys variable
            std::function<void()> &f = tasks.front();
            tasks.pop_front();
            //small race condition where a task is added after the if returns true and before all threads are waked
            // Every thread will then compete to acquire the lock which is bad for performance, has no other effect
            guard.unlock();
            //if the wrong thread is woken up, then the thread running join might not be waked and keep waiting
            //therefore must make sure to wake the potential thread running join()
            if(tasks.empty()){
                cv.notify_all();
            }
            else{
                //need to notify here and not just in post() in-case more tasks are added than there are threads waiting
                cv.notify_one();
            }
            f();
        }
    }

    //TODO "Legg til en Workers metode stop som avslutter workers trådene for eksempel når task-listen er tom." ??
    void stop() {

    }
    
    //can't get this to work with a pointer to the function, but c'est la vie
    void post_timeout(std::function<void()> func, int ms) {
        std::function<void()> f = [&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            func();
        };
        post(&f);
    }

    void join() {
        //ensure that no more tasks will be added while joining threads
        std::unique_lock<std::mutex> guard(task_lock);
        if(!tasks.empty()){
            cv.wait(guard, [this]{
                //only want to wake up if all tasks are finished
                return tasks.empty();
            });
        }
        //no more tasks will be run after this, even if they are added, safe to unlock
        cont = false;
        guard.unlock();
        //every thread will need to finish
        cv.notify_all();
        for (int i = 0; i < numThreads; ++i) {
            if(threads[i].joinable()){
                threads[i].join();
            }
        }
    }
};
