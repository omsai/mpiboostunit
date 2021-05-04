#include "mpi_bandwidth_util.hpp"

void pair_src_dest(int rank, int numtasks, int* src, int* dest) {
  /* Determine who my send/receive partner is */
  if (rank < numtasks/2)
    *dest = *src = numtasks/2 + rank;
  if (rank >= numtasks/2)
    *dest = *src = rank - numtasks/2;
}
