/****************************************************************************
* FILE: mpi_bandwidth.cpp
* DESCRIPTION:
*   Provides point-to-point communications timings for any even
*   number of MPI tasks.
* CHANGELOG:
*   Overhauled to use Boost C++ API per:
*   https://www.boost.org/doc/libs/1_76_0/doc/html/mpi/tutorial.html
* AUTHOR: Blaise Barney
* MAINTAINER: Pariksheet Nanda
* LAST REVISED: 05/03/21
****************************************************************************/
#include "mpi_bandwidth_util.hpp"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/timer.hpp>
#include <boost/serialization/string.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
namespace mpi = boost::mpi;
const int ERR_UNEVEN_MPI_NUM_TASKS = 3;

#define MAXTASKS      8192
/* Change the next four parameters to suit your case */
#define STARTSIZE     100000
#define ENDSIZE       1000000
#define INCREMENT     100000
#define ROUNDTRIPS    100

int main (int argc, char *argv[])
{
int     numtasks, rank, n, i, j, rndtrps, nbytes, start, end, incr,
        src, dest, rc, tag=1;
std::vector<int> taskpairs;
double  thistime, bw, bestbw, worstbw, totalbw, avgbw,
        bestall, avgall, worstall,
        timings[MAXTASKS/2][3], tmptimes[3],
        resolution;
mpi::timer timer;
char msgbuf[ENDSIZE];
std::string host;
std::vector<std::string> hostmap;

/* Some initializations and error checking */
mpi::environment env(argc, argv);
mpi::communicator world;
numtasks = world.size();
if (numtasks % 2 != 0) {
  printf("ERROR: Must be an even number of tasks!  Quitting...\n");
  rc = ERR_UNEVEN_MPI_NUM_TASKS;
  world.abort(rc);
  exit(rc);
  }
rank = world.rank();
start = STARTSIZE;
end = ENDSIZE;
incr = INCREMENT;
rndtrps = ROUNDTRIPS;
for (i=0; i<end; i++)
  msgbuf[i] = 'x';

/* All tasks send their host name to task 0 */
host = env.processor_name();
gather(world, host, hostmap, 0);

/* Determine who my send/receive partner is and tell task 0 */
pair_src_dest(rank, numtasks, &src, &dest);
gather(world, dest, taskpairs, 0);

if (rank == 0) {
  resolution = timer.elapsed_min();
  printf("\n******************** MPI Bandwidth Test ********************\n");
  printf("Message start size= %d bytes\n",start);
  printf("Message finish size= %d bytes\n",end);
  printf("Incremented by %d bytes per iteration\n",incr);
  printf("Roundtrips per iteration= %d\n",rndtrps);
  printf("MPI_Wtick resolution = %e\n",resolution);
  printf("************************************************************\n");
  for (i=0; i<numtasks; i++)
    std::cout << "task " << std::setw(4) << i << std::setw(0)
	      << " is on " << hostmap[i]
	      << " partner=" << taskpairs[i] << std::endl;
  printf("************************************************************\n");
  }


/*************************** first half of tasks *****************************/
/* These tasks send/receive messages with their partner task, and then do a  */
/* few bandwidth calculations based upon message size and timings.           */

if (rank < numtasks/2) {
  for (n=start; n<=end; n=n+incr) {
    bestbw = 0.0;
    worstbw = .99E+99;
    totalbw = 0.0;
    nbytes =  sizeof(char) * n;
    for (i=1; i<=rndtrps; i++){
      timer.restart();
      world.send(dest, tag, msgbuf, n);
      world.recv(src, tag, msgbuf, n);
      thistime = timer.elapsed();
      bw = ((double)nbytes * 2) / thistime;
      totalbw = totalbw + bw;
      if (bw > bestbw ) bestbw = bw;
      if (bw < worstbw ) worstbw = bw;
      }
    /* Convert to megabytes per second */
    bestbw = bestbw/1000000.0;
    avgbw = (totalbw/1000000.0)/(double)rndtrps;
    worstbw = worstbw/1000000.0;

    /* Task 0 collects timings from all relevant tasks */
    if (rank == 0) {
      /* Keep track of my own timings first */
      timings[0][0] = bestbw;
      timings[0][1] = avgbw;
      timings[0][2] = worstbw;
      /* Initialize overall averages */
      bestall = 0.0;
      avgall = 0.0;
      worstall = 0.0;
      /* Now receive timings from other tasks and print results. Note */
      /* that this loop will be appropriately skipped if there are    */
      /* only two tasks. */
      for (j=1; j<numtasks/2; j++)
        world.recv(j, tag, timings[j], 3);
      printf("***Message size: %8d *** best  /  avg  / worst (MB/sec)\n",n);
      for (j=0; j<numtasks/2; j++) {
        printf("   task pair: %4d - %4d:    %4.2f / %4.2f / %4.2f \n",
              j, taskpairs[j], timings[j][0], timings[j][1], timings[j][2]);
        bestall += timings[j][0];
        avgall += timings[j][1];
        worstall += timings[j][2];
        }
      printf("   OVERALL AVERAGES:          %4.2f / %4.2f / %4.2f \n\n",
            bestall/(numtasks/2), avgall/(numtasks/2), worstall/(numtasks/2));
      }
    else {
      /* Other tasks send their timings to task 0 */
      tmptimes[0] = bestbw;
      tmptimes[1] = avgbw;
      tmptimes[2] = worstbw;
      world.send(0, tag, tmptimes, 3);
      }
    }
  }



/**************************** second half of tasks ***************************/
/* These tasks do nothing more than send and receive with their partner task */

if (rank >= numtasks/2) {
  for (n=start; n<=end; n=n+incr) {
    for (i=1; i<=rndtrps; i++){
      world.recv(src, tag, msgbuf, n);
      world.send(dest, tag, msgbuf, n);
      }
    }
  }


return 0;

}  /* end of main */
