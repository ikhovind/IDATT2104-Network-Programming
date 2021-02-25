#include <iostream>
#include "Workers.cpp"

int main() {
    Workers worker_threads(4);

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
    worker_threads.post(func);
    worker_threads.post(func2);
    worker_threads.post_timeout_epoll(func5, 1000);
    worker_threads.post_timeout_epoll(func6, 999);
    std::this_thread::sleep_for(std::chrono::seconds(7));
    worker_threads.join();

    return 0;
}
