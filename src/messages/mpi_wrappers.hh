#pragma once

#include <mpi.h>

#include "message.hh"

void send(const MessageNS::Message &message, int destination_rank);
void sendAll(const MessageNS::Message &message, int source_rank, int world_size);
json recv(MPI_Status &status);
std::optional<MPI_Status> checkForMessage(int source = MPI_ANY_SOURCE);
json waitForResponse(int rank);