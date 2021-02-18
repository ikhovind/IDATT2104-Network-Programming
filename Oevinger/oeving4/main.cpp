#include <iostream>
#include "Workers.cpp"


int main() {
    Workers worker_threads(4);

    std::function<void()> func = [&] {
        std::cout<<"this is the parameter function"<<std::endl;
    };
    
    worker_threads.start();
    worker_threads.post(&func);
    worker_threads.join();
    return 0;
}
