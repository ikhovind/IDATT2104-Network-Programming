//
// Created by ingebrigt on 17.02.2021.
//

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <list>
#include <mutex>
#include "condition_variable"

class Workers{

    std::condition_variable cv;
    std::list<std::function<void()>> tasks;
    std::mutex task_lock;
    std::vector<std::thread> threads;

public:
    Workers(int numThreads){
        threads = std::vector<std::thread>(numThreads);
    }

    //ta inn funksjoner som parameter pog
    void post(std::function<void()> *f){
        //guarantees that the lock will be unlocked when leaving scope
        std::unique_lock<std::mutex> guard(task_lock);
        tasks.emplace_back(*f);
        guard.unlock();
        //notify one thread as one possible task has been added
        cv.notify_one();
    }

    void start(){
        for(auto &t : threads){
            //want to run forever until join() is called
            while(true){
                //references same mutex so will be locked at same time as mutex in post()
                //unlocked when
                std::unique_lock<std::mutex> guard(task_lock);
                this->cv.wait(guard, [this]() {
                        return tasks.empty();
                    }
                );
                std::function<void()> f = tasks.front();
                guard.unlock();
                f();
            }
        }
        //kjør venting eller noe slikt som han viste i timen mens man venter på at post() kjører??
    }

    void post_timeout(){

    }

    void join(){
        for(auto &thread : threads){
            thread.join();
        }
    }
};
