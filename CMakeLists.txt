cmake_minimum_required(VERSION 3.3)

project("RFD__modelling")
set(utils ./utilities)
set(ccgsl ./ccgsl)

include_directories ("${utils}")
include_directories ("${ccgsl}")
include_directories ("C:/Program Files (x86)/CodeBlocks/MinGW/include")

SET(CMAKE_CXX_FLAGS "-std=c++0x -fmax-errors=10 -Wall -lgsl -lgslcblas -lm")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


add_executable(lehtinen_1999
              ./lehtinen_1999.cpp)