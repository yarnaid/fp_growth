cmake_minimum_required(VERSION 2.8)
project(fp_growth)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCES fptree.cpp)

add_library(fp_growth STATIC ${SOURCES})