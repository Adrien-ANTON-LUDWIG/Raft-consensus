#pragma once

#include <mpi.h>

#include <chrono>
#include <iostream>
#include <queue>

#include "worlds_info.hh"

#include "json.hpp"
#include "messages/mpi_wrappers.hh"
#include "repl_process.hh"
#include "utils/logs.hh"

using json = nlohmann::json;

enum STATE { FOLLOWER, CANDIDATE, LEADER };

class Server : ::REPL::Process {
 private:
  Universe m_universe;

  STATE m_state = FOLLOWER;

  int m_term = 0;

  /**
   * Election
   */
  // Id of the server this server voted for. Can be himself if candidate.
  int m_voted_for = -1;
  // If candidate, number of vote received.
  int m_vote_count = 0;

  /**
   * Log replication
   */
  // Id of the current leader.
  int m_leaderId;

  // Logs manager
  Logs m_logs = Logs();

  // For each server, index of the next log entry to send to that server.
  std::vector<int> m_nextIndex;

  // For each server, index of highest log entry known to be replicated on
  // server.
  std::vector<int> m_matchIndex;

  /**
   * Time management
   */
  std::chrono::milliseconds m_election_timeout;
  std::chrono::milliseconds m_heartbeat_timeout;
  std::chrono::_V2::system_clock::time_point m_start_time;
  std::chrono::_V2::system_clock::time_point m_current_time;

 public:
  Server(Universe universe, int replRank);

  /// @brief Main loop of the server
  void run();

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
  void handleAppendEntriesResponse(const json& json);

  // CMD HANDLERS
  void handleLoad(const json& json);

  // REPL HANDLERS
  virtual void handleREPLInfo(const json& json) override;

  // UTILS
  void dropMessage(const MessageNS::Message& message);
  void checkTerm(int term);
};
