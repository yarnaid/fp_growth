cmake_minimum_required(VERSION 2.8)

project (main)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_library (fptree FP-growth/fptree)
add_library (utils utils/io utils/utils utils/campaign_marker utils/validation_objective)
add_library (libs lib/train lib/labeling lib/graph_correlation)
add_library (data_model models/item models/fpnode models/transaction models/ip models/url)


add_executable (main main.cpp)
target_link_libraries (main libs fptree data_model utils)
