cmake_minimum_required(VERSION 2.8)

project(libs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH} ../dclib-code/dlib/all/source.cpp")
set(SOURCES labeling train graph_correlation)

add_library(libs STATIC ${SOURCES})
