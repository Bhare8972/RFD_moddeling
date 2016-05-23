cmake_minimum_required(VERSION 2.8)

project("makeTables")
set(utils ../utils)
set(ccgsl ../ccgsl)

include_directories ("${utils}")
include_directories ("${ccgsl}")

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--no-as-needed,-u,pthread_join,-u,pthread_equal")


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_executable(make_diffusion_tables
              ./make_diffusion_tables.cpp)
target_link_libraries(make_diffusion_tables gsl gslcblas pthread)

add_executable(make_moller_tables
              ./make_moller_tables.cpp)
target_link_libraries(make_moller_tables gsl gslcblas pthread)

