#pragma once

#include <mpi.h>

#include "message.hh"

void send(const MessageNS::Message &message, int destination_rank, MPI_Comm channel);
void sendAll(const MessageNS::Message &message, int source_rank, int world_size, MPI_Comm channel);
json recv(MPI_Status &status, MPI_Comm channel);
std::optional<MPI_Status> checkForMessage(MPI_Comm channel, int source = MPI_ANY_SOURCE);

/// @brief Check for response and retrieve it if arrived
/// @param rank 
/// @param channel 
/// @param responseData 
/// @return True if function should be called again as response is not yet arrived
bool waitForResponse(int rank, MPI_Comm channel, json& responseData);