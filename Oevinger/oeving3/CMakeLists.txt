cmake_minimum_required(VERSION 3.17)
project(Oeving3)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -O3 -march=native")
add_executable(Oeving3 Erling&Ingebrigt.cpp)
