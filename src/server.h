#pragma once

#include <mpi.h>

#include <chrono>
#include <iostream>
#include <queue>

#include "json.hpp"
#include "messages/mpi_wrappers.hh"
#include "repl_process.hh"

using json = nlohmann::json;

enum STATE { FOLLOWER, CANDIDATE, LEADER };

class Server : REPL::Process {
 private:
  int m_id;  // Corresponding to MPI rank

  STATE m_state = FOLLOWER;

  int m_term = 0;

  // Election
  // Id of the server this server voted for. Can be himself if candidate.
  int m_voted_for = -1;
  // If candidate, number of vote received.
  int m_vote_count = 0;

  // MPI management
  int m_world_size;

  // Clients
  std::queue<json> m_commands;

  // Time management
  std::chrono::milliseconds m_election_timeout;
  std::chrono::milliseconds m_heartbeat_timeout;
  std::chrono::_V2::system_clock::time_point m_start_time;
  std::chrono::_V2::system_clock::time_point m_current_time;

  // REPL

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

  // REPL HANDLERS
  virtual void handleREPLInfo(const json& json) override;

  // UTILS
  void dropMessage(const MessageNS::Message& message);
  void checkTerm(int term);
};
