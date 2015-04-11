cmake_minimum_required(VERSION 2.8)
project(utils)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "../cpp-netlib")
set(CPP-NETLIB_DIR "../cpp-netlib")
find_package(cppnetlib REQUIRED)
include_directories(${CPP-NETLIB_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

set(SOURCES io utils ../cpp-netlib)

add_library(utils STATIC ${SOURCES})
