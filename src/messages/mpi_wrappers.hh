#pragma once

#include <mpi.h>

#include "message.hh"

void send(const MessageNS::Message &message, int destination_rank, MPI_Comm channel);
void sendAll(const MessageNS::Message &message, int source_rank, int world_size, MPI_Comm channel);
json recv(MPI_Status &status, MPI_Comm channel);
std::optional<MPI_Status> checkForMessage(MPI_Comm channel, int source = MPI_ANY_SOURCE);
json waitForResponse(int rank, MPI_Comm channel);