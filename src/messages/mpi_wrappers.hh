#pragma once

#include <mpi.h>

#include "message.hh"

void send(Message::Message &message, int destination_rank);
void sendAll(Message::Message &message, int source_rank, int world_size);
json recv(MPI_Status &status);
std::optional<MPI_Status> checkForMessage();
