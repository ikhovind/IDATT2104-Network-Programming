cmake_minimum_required(VERSION 3.17)
project(oeving4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -pthread")

add_executable(oeving4 main.cpp Workers.cpp)