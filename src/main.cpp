#include <mpi.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

#include "client.hh"
#include "server.h"
#include "spdlog/spdlog.h"
#include "repl.hh"

int main(int argc, char **argv) {

  // Log setup
  spdlog::set_level(spdlog::level::off);
  spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

  /////////////////////////////////////////////////////////////////

  if (argc < 3) {
    std::cerr << "Invalid argument count. Received " << argc - 1 << ", expected 2." << std::endl;
    return 1;
  }
  
  int client_count;
  int server_count;
  try {
    client_count = std::stoi(argv[1]);
    server_count = std::stoi(argv[2]);
  }
  catch (std::exception e) {
    std::cerr << "Invalid arguments." << std::endl << e.what() << std::endl;
    return 1;
  }

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

  if (my_rank < client_count)
  {
    Client client(my_rank, world_size);
    if (argc == 4)
      client.loadCommands(argv[3]);

    while (true) {
      client.update();
    }
  }
  else if (my_rank < client_count + server_count)
  {
    Server server(my_rank, world_size);

    while (true) {
      server.update();
    }
  }
  else if (my_rank == client_count + server_count) {
    REPL::start();
  }

  /////////////////////////////////////////////////////////////////

  // Finalize MPI
  // This must always be called after all other MPI functions
  MPI_Finalize();

  return 0;
}
