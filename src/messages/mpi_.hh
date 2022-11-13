#pragma once

#include <mpi.h>

#include "message.hh"

void send(Message::Message &message, int destination_rank);
void send_all(Message::Message &message, int source_rank, int world_size);
std::optional<json> recv();
void recv_all(std::vector<json> &messages);
