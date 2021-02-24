#include <iostream>
#include "Workers.cpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main() {
    Workers worker_threads(1);

    std::function<void()> func = [&] {
        std::cout<<"worker function 1"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
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



    worker_threads.start();
    worker_threads.post(func);
    worker_threads.post(func2);
    worker_threads.post_timeout_epoll(func5, 1000);
    worker_threads.join();

    return 0;
}
