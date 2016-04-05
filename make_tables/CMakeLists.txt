cmake_minimum_required(VERSION 2.8)

project("makeTables")
set(utils ../utils)
set(ccgsl ../ccgsl)

include_directories ("${utils}")
include_directories ("${ccgsl}")

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fmax-errors=10 -Wall -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


add_executable(make_ionization_tables
              ./make_ionization_tables.cpp)
target_link_libraries(make_ionization_tables gsl gslcblas)

add_executable(make_diffusion_tables
              ./make_diffusion_tables.cpp)
target_link_libraries(make_diffusion_tables gsl gslcblas)

