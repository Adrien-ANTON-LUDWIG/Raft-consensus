#include <mpi.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

#include "worlds_info.hh"

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
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
  spdlog::flush_on(spdlog::level::info);

  /////////////////////////////////////////////////////////////////

  if (argc < 3) {
    std::cerr << "Invalid argument count. Received " << argc - 1
              << ", expected at least 2." << std::endl;
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

  /////////////////////////////////////////////////////////////////

  int replRank = client_count + server_count;

  Universe universe;
  universe.replWorld.com = MPI_COMM_WORLD;
  MPI_Comm_rank(MPI_COMM_WORLD, &universe.replWorld.rank);
  MPI_Comm_size(MPI_COMM_WORLD, &universe.replWorld.world_size);

  int rank = universe.replWorld.rank;
  if (rank < server_count)
  {
    // Adding server to inter-server communications channel
    MPI_Comm_split(MPI_COMM_WORLD, 0, rank, &universe.serverWorld.com);
    MPI_Comm_rank(universe.serverWorld.com, &universe.serverWorld.rank);
    universe.serverWorld.world_size = server_count;

    // Adding server to client->server communications channel
    MPI_Comm_split(MPI_COMM_WORLD, 1, rank, &universe.clientServerWorld.com);
    MPI_Comm_rank(universe.clientServerWorld.com, &universe.clientServerWorld.rank);
    universe.clientServerWorld.world_size = server_count + client_count;

    Server server(universe, replRank);

    server.run();
  }
  else if (rank < replRank)
  {
    // Mark as not participating to server communication channel creation
    MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &universe.clientServerWorld.com);

    // Adding client to client->server communications channel
    MPI_Comm_split(MPI_COMM_WORLD, 1, rank, &universe.clientServerWorld.com);
    MPI_Comm_rank(universe.clientServerWorld.com, &universe.clientServerWorld.rank);
    universe.serverWorld.world_size = server_count + client_count;

    // std::cout << "Client " << universe.replWorld.rank << ":\n";
    // std::cout << universe << std::endl;

    Client client(universe, server_count, replRank);
    if (argc == 5)
      client.loadCommands(argv[4]);

    client.run();
  }
  else if (rank == replRank) {
    // Mark as not participating to server communication channel creation
    MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &universe.clientServerWorld.com);

    // Mark as not participating to client-server communication channel creation
    MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &universe.clientServerWorld.com);

    REPL::init(universe, argc == 5 && argv[3][0] == '0' ? argv[4] : "");
    REPL::start(client_count, server_count);
  }

  /////////////////////////////////////////////////////////////////

  // Finalize MPI
  // This must always be called after all other MPI functions
  MPI_Finalize();

  return 0;
}