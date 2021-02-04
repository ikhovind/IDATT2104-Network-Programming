#include <iostream>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <math.h>
#include<bits/stdc++.h>

void findPrimes(std::vector<int> *result, int maxNum);

int currentNum;
std::mutex numLock;

int main(int argc, char* argv[]) {
    const int maxNum = atoi(argv[1]);
    int numThreads = atoi(argv[2]);
    ;
    std::vector<int> foundPrimes;

    for(int i = 0; i < maxNum; i += 1){
        foundPrimes.emplace_back(i);
    }


    foundPrimes[0] = -1;
    foundPrimes[1] = -1;
    currentNum = 2;

    std::thread threads[numThreads];


    std::chrono::milliseconds start = std::chrono::duration_cast< std::chrono::milliseconds 	>(std::chrono::system_clock::now().time_since_epoch());

    for(int i = 0; i < numThreads; i++){
        threads[i] = std::thread(findPrimes, &foundPrimes, maxNum);
    }

    for(int i = 0; i < numThreads; i++){
        threads[i].join();
    }
    std::chrono::milliseconds end = std::chrono::duration_cast< std::chrono::milliseconds 	>(std::chrono::system_clock::now().time_since_epoch());

    std::cout << "Used time: " << (end-start).count() << " ms" << std::endl;

    for (int i = 0; i < maxNum; i++){
        if(foundPrimes[i] != -1){
            //std::cout << foundPrimes[i] << " ";
        }
    }
    std::cout << std::endl;


    return 0;
}

void findPrimes(std::vector<int> *result, int maxNum) {
    int maxSquare = sqrt(maxNum);

    numLock.lock();
    int num = currentNum;
    currentNum += 1;
    numLock.unlock();

    while(num <= maxSquare){
        //starts from 2², removes every number that's divisible by n after this
        for(int i = num * num; i < maxNum; i += num){
            (*result)[i] = -1;
        }
        numLock.lock();
        //this is so that we wont e.g. remove numbers that are divisible by 4 after already having removed them with 2
        while((*result)[currentNum] == -1){
            currentNum+=1;
        }
        num = currentNum;
        currentNum += 1;
        numLock.unlock();
    }
}
