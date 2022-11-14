#include "server.h"

#include "messages/mpi_wrappers.hh"
#include "spdlog/spdlog.h"

Server::Server(int id, int world_size) {
  this->id = id;
  this->world_size = world_size;

  std::srand(id);
  election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
  heartbeat_timeout = std::chrono::milliseconds(50);
  start_time = std::chrono::system_clock::now();

  spdlog::info("{}: Election timeout: {}", id, election_timeout.count());
}

void Server::anyStateUpdate() {
  // TODO : Refer to Raft paper > Rules for Servers > All servers
}

void Server::followerUpdate() {
  if (current_time - start_time >= election_timeout) return becomeCandidate();

  // MPI_Status status = checkForMessage();

  // if (status && status->tag == TYPE)
  //   handler_type(status);
}

void Server::candidateUpdate() {}

void Server::leaderUpdate() {}

void Server::becomeCandidate() {}

void Server::update() {
  // Update the current_time
  current_time = std::chrono::system_clock::now();

  anyStateUpdate();

  switch (state) {
    case LEADER:
      leaderUpdate();
      break;
    case CANDIDATE:
      candidateUpdate();
      break;
    default:  // FOLLOWER
      followerUpdate();
      break;
  }
}