#include "messages/RPC/appendEntries.hh"
#include "messages/RPC/requestVote.hh"
#include "messages/RPC/vote.hh"
#include "messages/mpi_wrappers.hh"
#include "messages/status.hh"
#include "server.h"
#include "spdlog/spdlog.h"

using namespace MessageNS;

void Server::handleRequestVote(const json &json) {
  RPC::RequestVote request(json);
  spdlog::info("{}: Received request vote from {}", m_id, request.getCandidate());
  checkTerm(request.getTerm());

  bool grantVote = false;

  if (request.getTerm() >= m_term &&
      (m_voted_for == -1 || m_voted_for == request.getCandidate())) {
    grantVote = true;
    m_voted_for = request.getCandidate();
    
    // Reset election timeout
    m_start_time = std::chrono::system_clock::now();
  }

  RPC::Vote vote(m_term, grantVote, m_id);
  send(vote, request.getCandidate());
}

void Server::handleVote(const json &json) {
  RPC::Vote vote(json);
  checkTerm(vote.getTerm());

  // Check state after checkTerm
  if (m_state != CANDIDATE) {
    spdlog::info("{}: Received vote but not candidate", m_id);
    return;
  }

  if (vote.isGranted()) {
    m_vote_count++;
    spdlog::info("{}: Vote granted by {}", m_id, vote.getOriginId());
  }
  else {
    spdlog::info("{}: Vote denied by {}", m_id, vote.getOriginId());
  }
}

void Server::handleAppendEntries(const json &json) {
  RPC::AppendEntries log(json);

  if (log.getTerm() < m_term) {
    Status status(m_term, false, m_id);
    return send(status, log.getLeader());
  }

  if (log.isHeartbeat()) {
    spdlog::info("{}: Received heartbeat from {}", m_id, log.getLeader());
    m_start_time = std::chrono::system_clock::now();
  } else {
    spdlog::info("{}: Received append entries from {}", m_id, log.getLeader());
    // TODO check if log contains an entry at prevLogIndex whose term matches
    // prevLogTerm

    // TODO check if entry conflicts with a new one (same index but different
    // terms)

    // TODO append new entries not already in the log

    // if (log.getLeaderCommit() > this->commitIndex)
    // this->commitIndex = min(log.getLeaderCommit(), index of last new entry)
  }
}