cmake_minimum_required(VERSION 2.8)

project("RFD__modelling")
set(utils ./utilities)
set(ccgsl ./ccgsl)

include_directories ("${utils}")
include_directories ("${ccgsl}")



SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fmax-errors=10 -Wall -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


add_executable(lehtinen99
              ./lehtinen_1999.cpp)

target_link_libraries(lehtinen99 gsl gslcblas)
