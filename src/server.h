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
  Server(int id, int world_size);

  // GENERAL UPDATE
  void update();

  // FOLLOWER
  void becomeFollower();
  void followerUpdate();

  // ELECTION
  void becomeCandidate();
  void candidateUpdate();

  // LEADER
  void becomeLeader();
  void leaderUpdate();
  void sendHeartbeat();

  // RPC HANDLERS
  void handleRequestVote(const json& json);
  void handleVote(const json& json);
  void handleAppendEntries(const json& json);

  // CMD HANDLERS
  void handleLoad(const json& json);

  // UTILS
  void dropMessage(const MessageNS::Message& message);
  void checkTerm(int term);

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
  int vote_count = 0;

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
