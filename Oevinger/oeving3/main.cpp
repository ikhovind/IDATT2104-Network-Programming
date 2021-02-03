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
std::mutex vectorLock;
std::vector<int> foundPrimes;

int main() {
    currentNum = 3;
    foundPrimes.emplace_back(2);

    std::thread first(findPrimes, &foundPrimes, 99);
    std::thread second(findPrimes, &foundPrimes, 99);

    first.join();
    second.join();

    std::cout << std::endl;
    for (auto it = foundPrimes.begin() ; it != foundPrimes.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';

    return 0;
}

void addNum(std::vector<int> *resultList, int num){
    vectorLock.lock();
    resultList->emplace_back(num);
    vectorLock.unlock();
}

void findPrimes(std::vector<int> *result, int maxNum) {
    numLock.lock();
    int num = currentNum;
    currentNum += 2;
    numLock.unlock();

    while (num <= maxNum) {
        bool prime = true;
        int maxCheck = (int) sqrt(num);
        //checks if divisible by odd numbers up to it's own square root
        for (int i = 3; i <= maxCheck; i += 2) {
            if (std::__gcd(num, i) != 1) {
                prime = false;
                break;
            }
        }
        if(prime){
            addNum(result, num);
        }


        numLock.lock();
        num = currentNum;
        currentNum += 2;
        numLock.unlock();

        /*
         for hvert tall i result
            hvis tallet er mindre enn eller lik maxCheck
                sjekk om det har noen felles faktorer med num
            hvis tallet er større enn maxcheck
                addNum(&result, num);
                break;

         */

        //kun oddetall
        //trenger kun å sjekke tall som er mindre enn sqrt(n), da tall over dette vil ganges med et tall som er mindre enn seg selv for å få n, og da vil vi finne det minste tallet først
        //trenger kun å sjekke om det er delelig på primtall, da man alle andre tall har mindre primtallsfaktorer

        /*
         * måter tråder kan brukes på:
         *  - bruke en på lave primtall og en på høye
         *      - de høye tallene må da være primtall som er et sted mellom 2² til (høyeste primtall funnet)²
         *  - sjekke hvert og hvert tall
         *      - mens tråd en sjekker 3 så sjekker tråd 2 5
         *      - trenger da en lås som gjør at de ikke endrer tall samtidig
         *          - ha et heltall som starter på 3 og inkrementeres med 2 og 2, men låses
         *          - les først, så inkrementer sløik at neste tråd leser den inkrementerte verdien
         */

    }
}
