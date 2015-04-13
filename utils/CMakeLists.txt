cmake_minimum_required(VERSION 2.8)
project(utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
include_directories(../dclib-code/dlib/all/source.cpp)
#set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH} ../dclib-code/dlib/all/source.cpp")

set(SOURCES io utils campaign_marker validation_objective)

add_library(utils STATIC ${SOURCES})
