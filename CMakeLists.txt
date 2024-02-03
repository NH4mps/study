cmake_minimum_required(VERSION 3.20)

project(Study CXX)

set(CMAKE_CXX_STANDARD 11)

add_executable(tuple tuple.cpp)

add_executable(foreach foreach.cpp)

add_executable(pievewice_construct pievewice_construct.cpp)

add_executable(swap swap.cpp)

add_executable(optional optional.cpp)

add_executable(variant variant.cpp)