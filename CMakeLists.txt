cmake_minimum_required(VERSION 3.22)
project(gen)

set(CMAKE_CXX_STANDARD 20)

add_executable(gen main.cpp GenNode.cpp)