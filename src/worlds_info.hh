#pragma once

#include <iostream>

#include <mpi.h>

struct WorldInfo {
  MPI_Comm com = MPI_COMM_WORLD;
  int rank = 0;
  int world_size = 0;
};

std::ostream& operator<<(std::ostream& os, const WorldInfo& wi);

struct Universe {
  WorldInfo serverWorld;        // World used for inter-server communications
  WorldInfo clientServerWorld;  // World used for client<->server communications
  WorldInfo replWorld;          // World used for REPL->processes communications
};

std::ostream& operator<<(std::ostream& os, const Universe& u);