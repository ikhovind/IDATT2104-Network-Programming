#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <list>
#include <mutex>
#include "condition_variable"
#include <sys/epoll.h>
#include <sys/timerfd.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
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

    void post(const std::function<void()>& f) {
        //unique guarantees that the lock will be unlocked when leaving function scope
        std::unique_lock<std::mutex> guard(task_lock);
        tasks.emplace_back(f);
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
        while (cont || !tasks.empty()) {
            //references same mutex so will be locked at same time as mutex in post()
            std::unique_lock<std::mutex> guard(task_lock);
            cv.wait(guard, [this]{return !tasks.empty() || !cont;});
            if(!cont && tasks.empty()) return;
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
    
    //This is how i interpreted the original post_timeout function to work, that is the waiting not happening until
    //it is picked out of the queue, se @post_timeout_epoll for different interpretation
    void post_timeout(const std::function<void()> &func, int ms) {
        //waiting happens after thread is picked out of queue, this is how the teaching assistant interpreted the task
        std::function<void()> f = [func, ms]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            func();
        };
        post(f);
    }

    //here the waiting happens before the thread is posted
    void post_timeout_epoll(std::function<void()> func, int ms){
        int epoll_fd = epoll_create1(0); //returns an integer refering to an open file
        //0 is ize of file
        epoll_event timeout; //struct containing 32 bit int for data and 32 bit int for event
        timeout.events = EPOLLIN; //EPOLLIN notifies when the file is available for reading
        timeout.data.fd = timerfd_create(CLOCK_MONOTONIC, 0);  //create new timer-file and save the file descriptor
        //timeout.events is what events we want to be notified for
        //data.fd is what files we want to monitor for the aforementioned events
        itimerspec ts; //struct that specifies  new initial value and new interval for the timer
        //replaces the timer that is created in the timerfd_create file
        ts.it_value.tv_sec = ms / 1000; // Delay before initial event in seconds
        //e.g. with 2123 ms it becomes 2 seconds
        ts.it_value.tv_nsec = (ms % 1000) * 1000000;
        //The number of nanoseconds elapsed since the time given by the tv_sec member.
        //used to get accuracy that is lost when converting to whole seconds
        //e.g. with 2123 ms it becomes 123 * 1 000 000 = 123 000 000 nanoseconds = 0.123 seconds
        //seems like it has to be < 1.0 for some reason, waits forever when set to anything above 1
        ts.it_interval.tv_sec = 0; // Period between repeated events after initial delay (0: disabled)
        ts.it_interval.tv_nsec = 0; // Period between repeated events after initial delay (0: disabled)
        timerfd_settime(timeout.data.fd, 0, &ts, nullptr); //arms timer with the times given above

        // Add timeout to epoll so that it is monitored by epoll_wait:
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timeout.data.fd, &timeout);

        threads.insert(threads.begin()+numThreads,
        std::thread([this, func, epoll_fd, timeout](){
            //buffer used for storing events
            std::vector<epoll_event> events(128); // Max events to process at once'
            do {
                //returns when the file is available for reading, thread is stopped until epoll_wait returns
                //epoll_wait returns the number of file descriptors that are ready when epoll_wait returns
                int event_count = epoll_wait(epoll_fd, events.data(), events.size(), -1);
                for (int i = 0; i < event_count; i++) {
                    //if the file descriptor of the file belonging to the returned event is the same as the fd of the above
                    //timeout we're waiting for
                    //what if two timers are set in quick sucession such that the first is not finished by the time the
                    //second finishes??
                    //wont timeout.data.fd have changed?
                    if (events[i].data.fd == timeout.data.fd) {
                        // Remove timeout from epoll so that it is no longer monitored by epoll_wait:
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, timeout.data.fd, nullptr);
                        post(func);
                    }
                }
            } while(cont);
        })
        );

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
        for (int i = 0; i < threads.size(); ++i) {
            if(threads[i].joinable()){
                threads[i].join();
            }
        }
    }
};

#pragma clang diagnostic pop