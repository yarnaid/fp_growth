cmake_minimum_required(VERSION 2.8)

project (main)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library (fptree FP-growth/fptree)
add_library (utils utils/io utils/utils)
add_library (libs lib/train lib/labeling)


add_executable (main main.cpp)
target_link_libraries (main fptree utils libs)