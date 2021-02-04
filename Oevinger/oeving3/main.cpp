#include <iostream>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <math.h>
#include<bits/stdc++.h>

void findPrimes();

int currentNum;
const int maxNum = 100;
std::mutex numLock;
int numArr[maxNum-1];

int main() {
    for(int i = 0; i < maxNum; i += 1){
        numArr[i] = i;
    }
    numArr[0] = NULL;
    numArr[1] = NULL;
    currentNum = 2;

    std::thread first(findPrimes);
    std::thread second(findPrimes);

    first.join();
    second.join();

    for (int i = 0; i < maxNum; i++){
        if(numArr[i] != NULL){
            std::cout << numArr[i] << " ";
        }
    }
    std::cout << std::endl;


    return 0;
}

void findPrimes() {
    int maxSquare = sqrt(maxNum);

    numLock.lock();
    int num = currentNum;
    currentNum += 1;
    numLock.unlock();

    while(num <= maxSquare){
        //starts from 2², removes every number that's divisible by n after this
        for(int i = num * num; i < maxNum; i += num){
            numArr[i] = NULL;
        }
        numLock.lock();
        //this is so that we wont e.g. remove numbers that are divisible by 4 after already having removed them with 2
        while(numArr[currentNum] == NULL){
            currentNum+=1;
        }
        num = currentNum;
        currentNum += 1;
        numLock.unlock();
    }
}
