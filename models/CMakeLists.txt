cmake_minimum_required(VERSION 2.8)
project(data_model)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCES item fpnode)

add_library(data_model STATIC ${SOURCES})
