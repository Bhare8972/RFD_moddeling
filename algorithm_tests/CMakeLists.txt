cmake_minimum_required(VERSION 2.8)

project("alg_tsts")
set(utils ../utils)
set(ccgsl ../ccgsl)
set(tables ../read_tables)

include_directories ("${utils}")
include_directories ("${ccgsl}")
include_directories ("${tables}")

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fmax-errors=10 -Wall -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


add_executable(tst_fast_isq
              ./test_fast_inverse_sq.cpp)
target_link_libraries(tst_fast_isq gsl gslcblas)

add_executable(tst_elastic_scatt
              ./SC_elastic_scatt_tst.cpp)
target_link_libraries(tst_elastic_scatt gsl gslcblas)

#add_executable(lookup_speed_test
#              ./lookup_speed_test.cpp)
#target_link_libraries(lookup_speed_test gsl gslcblas)

add_executable(lookup_speed_test_two
              ./lookup_speed_test_two.cpp)
target_link_libraries(lookup_speed_test_two gsl gslcblas)
