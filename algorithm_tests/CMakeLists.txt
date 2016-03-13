cmake_minimum_required(VERSION 2.8)

project("alg_tsts")
set(utils ../utilities)
set(ccgsl ../ccgsl)

include_directories ("${utils}")
include_directories ("${ccgsl}")



SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fmax-errors=10 -Wall -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


add_executable(tst_fast_isq
              ./test_fast_inverse_sq.cpp)

target_link_libraries(tst_fast_isq gsl gslcblas)
