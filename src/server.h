#pragma once

#include <mpi.h>

#include <chrono>
#include <iostream>
#include <queue>

#include "messages/mpi_wrappers.hh"

#include "json.hpp"

using json = nlohmann::json;

enum STATE { FOLLOWER, CANDIDATE, LEADER };

class Server {
 public:
  Server(int id, int world_size) {
    this->id = id;
    this->world_size = world_size;

    std::srand(id);
    election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
    heartbeat_timeout = std::chrono::milliseconds(50);
    start_time = std::chrono::system_clock::now();

    std::cout << id << ": Election timeout: " << election_timeout.count()
              << std::endl;
  }

  void anyStateUpdate();
  void followerUpdate();
  void candidateUpdate();
  void leaderUpdate();
  void becomeFollower();
  void becomeCandidate();
  void becomeLeader();
  void update();

  void handleRequestVote(const json& json);
  void handleVote(const json& json);
  void handleAppendEntries(const json& json);

  /**
   * Variables
   */

  int id;  // Corresponding to MPI rank

  STATE state = FOLLOWER;

  int term = 0;

  // Election
  // Id of the server this server voted for. Can be himself if candidate.
  int voted_for = -1;
  // If candidate, number of vote received.
  int vote_count = -1;

  // MPI management
  int world_size;

  // Clients
  std::queue<json> commands;

  // Time management
  std::chrono::milliseconds election_timeout;
  std::chrono::milliseconds heartbeat_timeout;
  std::chrono::_V2::system_clock::time_point start_time;
  std::chrono::_V2::system_clock::time_point current_time;
};
