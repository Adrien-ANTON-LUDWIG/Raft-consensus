#include <mpi.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

#include "client.hh"
#include "repl.hh"
#include "server.hh"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

int main(int argc, char** argv) {
  // Log setup
  auto logger =
      spdlog::basic_logger_mt("basic_logger", "./build/logs.txt", true);
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::info);
  spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
  spdlog::flush_on(spdlog::level::info);

  /////////////////////////////////////////////////////////////////

  if (argc < 3) {
    std::cerr << "Invalid argument count. Received " << argc - 1
              << ", expected 2." << std::endl;
    return 1;
  }

  int client_count;
  int server_count;
  try {
    client_count = std::stoi(argv[1]);
    server_count = std::stoi(argv[2]);
  } catch (std::exception& e) {
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

  int replRank = client_count + server_count;
  if (my_rank < server_count)
  {
    Server server(my_rank, world_size, replRank);

    while (true) {
      server.update();
    }
  }
  else if (my_rank < replRank)
  {
    Client client(my_rank, world_size, replRank);
    if (argc == 4)
      client.loadCommands(argv[3]);

    while (true) {
      client.update();
    }
  }
  else if (my_rank == replRank) {
    REPL::init(my_rank);
    REPL::start(client_count, server_count);
  }

  /////////////////////////////////////////////////////////////////

  // Finalize MPI
  // This must always be called after all other MPI functions
  MPI_Finalize();

  return 0;
}