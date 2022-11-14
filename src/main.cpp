#include <mpi.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "server.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) {

  // Log setup
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

  /////////////////////////////////////////////////////////////////

  // Initialize MPI
  // This must always be called before any other MPI functions
  MPI_Init(&argc, &argv);

  // Get the number of processes in MPI_COMM_WORLD
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of this process in MPI_COMM_WORLD
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /////////////////////////////////////////////////////////////////

  Server server(my_rank, world_size);

  while (true) {
    server.update();
  }

  /////////////////////////////////////////////////////////////////

  // Finalize MPI
  // This must always be called after all other MPI functions
  MPI_Finalize();

  return 0;
}
