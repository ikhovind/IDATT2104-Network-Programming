#include <iostream>
#include "Workers.cpp"

int main() {
    Workers worker_threads(4);
    Workers event_loop(1);
    std::function<void()> func = [&] {
        std::cout<<"worker function 1"<<std::endl;
    };

    std::function<void()> func2 = [&] {
        std::cout<<"worker function 2"<<std::endl;
    };
    std::function<void()> func3 = [&] {
        std::cout<<"event function 1"<<std::endl;
    };

    std::function<void()> func4 = [&] {

        std::cout<<"event function 2"<<std::endl;
    };

    std::function<void()> func5 = [&] {
        std::cout<<"after sleeping function"<<std::endl;
    };

    std::function<void()> func6 = [&] {
        std::cout<<"after sleeping function  2"<<std::endl;
    };

    worker_threads.start();
    event_loop.start();

    worker_threads.post(func);
    worker_threads.post(func2);

    event_loop.post(func3);
    event_loop.post(func4);

    worker_threads.post_timeout_epoll(func5, 1000);
    worker_threads.post_timeout_epoll(func6, 999);

    worker_threads.join();
    event_loop.join();
    return 0;
}
