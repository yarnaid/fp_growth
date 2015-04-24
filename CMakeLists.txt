cmake_minimum_required(VERSION 2.8)

project (main CXX)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
        message(FATAL_ERROR "Insufficient gcc version, update to >=4.9")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6")
        message(FATAL_ERROR "Insufficient clang version, update to >=6")
    endif()
endif()

add_library (fptree FP-growth/fptree)
add_library (utils utils/io utils/utils utils/campaign_marker utils/validation_objective)
add_library (libs lib/train lib/labeling lib/graph_correlation lib/pseudodb lib/corr_graph)
add_library (data_model models/item models/fpnode models/transaction models/ip models/url)


add_executable (main main.cpp)
target_link_libraries (main libs fptree data_model utils)
