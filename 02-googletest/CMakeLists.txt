cmake_minimum_required(VERSION 3.10)

project(mpi_bandwidth
  VERSION 20210503.0
  LANGUAGES CXX)

# Required by recent versions of boost.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(MPI REQUIRED)
find_package(Boost REQUIRED mpi serialization)

set(CMAKE_CXX_COMPILER "${MPI_CXX_COMPILER}")
add_executable(mpi_bandwidth mpi_bandwidth)
target_link_libraries(mpi_bandwidth ${Boost_LIBRARIES})
