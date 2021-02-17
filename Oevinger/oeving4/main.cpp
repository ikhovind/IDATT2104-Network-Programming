#include <iostream>
#include "Workers.cpp"

int main() {
    Workers worker_threads(4);
    Workers event_loop(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::function<void()> func = [&]{std::cout<<"test"<<std::endl;};
    worker_threads.start();
    worker_threads.post(&func);
    worker_threads.join();
    return 0;
}
