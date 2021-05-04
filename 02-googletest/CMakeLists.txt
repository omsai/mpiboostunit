cmake_minimum_required(VERSION 3.11) # FetchContent

project(mpi_bandwidth
  VERSION 20210503.0
  LANGUAGES CXX)

# C++11 is required by recent versions of boost as well as GoogleTest.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wno-long-long -pedantic")

# Dependencies.
find_package(MPI REQUIRED)
find_package(Boost REQUIRED mpi serialization)
# GoogleTest dependency.
include(FetchContent)
FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# FIXME: There's probably a nicer way of only using MPI_CXX_COMPILER
# on mpi_bandwidth so that mpi_bandwidth_util is compield with the
# default CXX compiler because the mpi_bandwidth_util does not have
# any MPI dependency.  But I'm not sure how to set a compiler on a
# specific executable.
set(CMAKE_CXX_COMPILER "${MPI_CXX_COMPILER}")
# Split utilities into separate non-MPI, non-Boost library for testing.
add_library(mpi_bandwidth_util mpi_bandwidth_util.cpp)
# Main executable.
add_executable(mpi_bandwidth mpi_bandwidth.cpp)
target_link_libraries(mpi_bandwidth mpi_bandwidth_util ${Boost_LIBRARIES})
install(TARGETS mpi_bandwidth DESTINATION bin)

# Tests.
enable_testing()
# Bandwidth calculations need paired workers.
add_test(NAME odd-np COMMAND mpirun -np 1 mpi_bandwidth)
set_tests_properties(odd-np PROPERTIES WILL_FAIL true)
add_test(NAME even-np COMMAND mpirun -np 2 mpi_bandwidth)
# Unit test the library with GoogleTest.
include(GoogleTest)
add_executable(test_mpi_bandwidth_util test_mpi_bandwidth_util.cpp)
target_link_libraries(test_mpi_bandwidth_util gtest_main mpi_bandwidth_util)
gtest_discover_tests(test_mpi_bandwidth_util)
