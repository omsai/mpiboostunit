CXX = mpic++
CXXFLAGS = -O2
# One must also link against boost_serialization per
# https://www.boost.org/doc/libs/1_76_0/doc/html/mpi/getting_started.html#mpi.getting_started.using
LDFLAGS = -lboost_mpi -lboost_serialization

mpi_bandwidth : mpi_bandwidth.cpp

# No need to write out the above rule; use GNU Make's built-in rule
# for compiling C programs:
# https://www.gnu.org/software/make/manual/html_node/Catalogue-of-Rules.html
