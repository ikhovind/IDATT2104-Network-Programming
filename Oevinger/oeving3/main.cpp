#include <iostream>
#include <vector>
 #include <mutex>
#include <thread>
#include <math.h>

void findPrimes(std::vector<bool> *result, int maxNum);

int currentNum;
//used to keep track of current number that is used to eliminate larger numbers
//lock to avoid one thread doing work that has already been done
std::mutex numLock;

int main(int argc, char* argv[]) {
    int minNum = atoi(argv[1]);
    const int maxNum = atoi(argv[2]);
    int numThreads = atoi(argv[3]);
    //true at index i if i is prime
    std::vector<bool> isPrime;

    for(int i = 0; i < maxNum; i += 1){
        isPrime.emplace_back(true);
    }


    isPrime[0] = false;
    isPrime[1] = false;
    currentNum = 2;

    std::thread threads[numThreads];


    std::chrono::milliseconds start = std::chrono::duration_cast< std::chrono::milliseconds 	>(std::chrono::system_clock::now().time_since_epoch());

    for(int i = 0; i < numThreads; i++){
        threads[i] = std::thread(findPrimes, &isPrime, maxNum);
    }

    for(int i = 0; i < numThreads; i++){
        threads[i].join();
    }
    std::chrono::milliseconds end = std::chrono::duration_cast< std::chrono::milliseconds 	>(std::chrono::system_clock::now().time_since_epoch());

    std::cout << "Used time: " << (end-start).count() << " ms" << std::endl;

    for (int i = 0; i < maxNum; i++){
        if(isPrime[i] && i >= minNum){
            std::cout <<i << " ";
        }
    }
    std::cout << std::endl;


    return 0;
}
/**
 * uses Sieve of Eratosthenes
 * https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 * @param maxNum the number you
 */
void findPrimes(std::vector<bool> *result, int maxNum) {
    //no need to check numbers after the square root of the max number, since no number under the max will be divisible by this
    int maxSquare = sqrt(maxNum);

    numLock.lock();
    int num = currentNum;
    currentNum += 1;
    numLock.unlock();

    while(num <= maxSquare){
        //starts from n², then removes every number from this to the max using steps of n
        //any number smaller than n² will have been removed already, e.g 2n is removed by 2, 3n by 3 and so on
        for(int i = num * num; i < maxNum; i += num){
            (*result)[i] = false;
        }
        numLock.lock();
        //this is so that we wont e.g. remove numbers that are divisible by 4 after already having removed them with 2
        while(!(*result)[currentNum]){
            currentNum+=1;
        }
        num = currentNum;
        currentNum += 1;
        numLock.unlock();
    }
}
