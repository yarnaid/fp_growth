cmake_minimum_required(VERSION 2.8)
project(utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

set(SOURCES io.cpp utils.cpp)

add_library(utils STATIC ${SOURCES})
