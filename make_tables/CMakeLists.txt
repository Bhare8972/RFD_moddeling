cmake_minimum_required(VERSION 2.8)

project("makeTables")
set(utils ../utils)
set(ccgsl ../ccgsl)

include_directories ("${utils}")
include_directories ("${ccgsl}")

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--no-as-needed,-u,pthread_join,-u,pthread_equal")


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-sign-compare" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

#add_executable(make_diffusion_tables_transform
#              ./make_diffusion_tables_transform.cpp)
#target_link_libraries(make_diffusion_tables_transform gsl gslcblas pthread)
#
#add_executable(make_diffusion_tables_MonteCarlo
#              ./make_diffusion_tables_MonteCarlo.cpp)
#3arget_link_libraries(make_diffusion_tables_MonteCarlo gsl gslcblas pthread)
#
#add_executable(combine_diffusion_tables
#              ./combine_diffusion_tables.cpp)
#target_link_libraries(combine_diffusion_tables gsl gslcblas)

add_executable(make_brem_tables
              ./make_brem_tables.cpp)
target_link_libraries(make_brem_tables gsl gslcblas)

add_executable(make_coulomb_tables
              ./make_diffusion_tables.cpp)
target_link_libraries(make_coulomb_tables gsl gslcblas pthread)


