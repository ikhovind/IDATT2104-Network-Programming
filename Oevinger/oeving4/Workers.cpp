#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <list>
#include <mutex>
#include "condition_variable"
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <atomic>


class Workers {
    bool cont = true;
    std::atomic<int> waiting = 0;
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
        std::unique_lock<std::mutex> guard(task_lock);
        tasks.emplace_back(f);
        guard.unlock();
        cv.notify_one();
    }

    void start() {
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back( std::thread{&Workers::consume, this} );
        }
    }

    void consume() {
        while (cont) {
            std::unique_lock<std::mutex> guard(task_lock);
            cv.wait(guard, [this]{return !tasks.empty() || !cont;});
            if(!cont) return;
            //deep copy as pop_front destroys variable
            std::function<void()> &f = tasks.front();
            tasks.pop_front();
            guard.unlock();
            //want to make sure the waiting join() thread is waked
            if(tasks.empty() && waiting == 0){
                cv.notify_all();
            }
            cv.notify_one();

            f();
        }
    }

    //chosen to interpret this as stop the threads even if there are tasks left, otherwise same functionality as join()
    void stop() {
        //ensure that no more tasks will be added while joining threads
        std::unique_lock<std::mutex> guard(task_lock);
        cont = false;
        guard.unlock();
        cv.notify_all();
        for (int i = 0; i < threads.size(); ++i) {
            if(threads[i].joinable()){
                threads[i].join();
            }
        }
    }
    
    //This is how i interpreted the original post_timeout function to work, that is the waiting not happening until
    //it is picked out of the queue, se post_timeout_epoll for different interpretation
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
        waiting.operator++();
        int epoll_fd = epoll_create1(0); //returns an integer referring to an open file
        epoll_event timeout;
        timeout.events = EPOLLIN; //EPOLLIN notifies when the file is available for reading
        timeout.data.fd = timerfd_create(CLOCK_MONOTONIC, 0);  //create new timer-file and save the file descriptor
        //timeout.events is what events we want to be notified for
        itimerspec ts; //struct that specifies  new initial value and new interval for the timer
        ts.it_value.tv_sec = ms / 1000; // Delay before initial event in seconds
        //e.g. with 2123 ms it becomes 2 seconds
        ts.it_value.tv_nsec = (ms % 1000) * 1000000;
        //The number of nanoseconds elapsed since the time given by the tv_sec member.
        //used to get accuracy that is lost when converting to whole seconds
        //e.g. with 2123 ms it becomes 123 * 1 000 000 = 123 000 000 nanoseconds = 0.123 seconds
        //seems like it has to be < 1.0 for some reason, waits forever when set to anything above 1
        ts.it_interval.tv_sec = 0; // Period between repeated events after initial delay (0: disabled)
        ts.it_interval.tv_nsec = 0; // Period between repeated events after initial delay (0: disabled)
        timerfd_settime(timeout.data.fd, 0, &ts, nullptr);

        // Add timeout to epoll so that it is monitored by epoll_wait:
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timeout.data.fd, &timeout);

        //TODO do you need a new thread for each epoll_wait, or can you wait for more than one file in the same thread?
        wait(func, epoll_fd, timeout);
    }

    void wait(const std::function<void()> &func, int epoll_fd, const epoll_event &timeout) {
        threads.insert(threads.begin() + numThreads++,std::thread([this, func, epoll_fd, timeout](){
            std::vector<epoll_event> events(128); // Max events to process at once'
                //returns when the file is available for reading, thread is stopped until epoll_wait returns
                int event_count = epoll_wait(epoll_fd, events.data(), events.size(), -1);
                for (int i = 0; i < event_count; i++) {
                    //if the file descriptor of the file belonging to the returned event is the same as the fd of the
                    // above timeout we're waiting for
                    if (events[i].data.fd == timeout.data.fd) {
                        // Remove timeout from epoll so that it is no longer monitored by epoll_wait:
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, timeout.data.fd, nullptr);
                        post(func);
                    }
                }
           waiting.operator--();
       }));
    }

    void join() {
        std::unique_lock<std::mutex> guard(task_lock);
        if(!tasks.empty() || waiting != 0){
            cv.wait(guard, [this]{
                if(tasks.empty() && waiting == 0){
                    //there are no tasks waiting and no epoll-waiting, we can safely wake and join threads
                    return true;
                }
                else{
                    //this thread was woken falsely, worker thread should wake instead
                    cv.notify_one();
                    return false;
                }
            });
        }
        cont = false;
        guard.unlock();
        cv.notify_all();
        for (int i = 0; i < threads.size(); ++i) {
            if(threads[i].joinable()){
                threads[i].join();
            }
        }
    }
};