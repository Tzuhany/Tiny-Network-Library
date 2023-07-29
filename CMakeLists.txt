cmake_minimum_required(VERSION 3.22)
project(zhnetlib)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

aux_source_directory(. SRC_LIST)
add_library(zhnetlib SHARED ${SRC_LIST})