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
install(TARGETS mpi_bandwidth DESTINATION bin)

enable_testing()

add_test(NAME odd-np COMMAND mpirun -np 1 mpi_bandwidth)
set_tests_properties(odd-np PROPERTIES WILL_FAIL true)

add_test(NAME even-np COMMAND mpirun -np 2 mpi_bandwidth)
