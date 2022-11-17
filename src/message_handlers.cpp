#include "messages/RPC/appendEntries.hh"
#include "messages/RPC/appendEntriesResponse.hh"
#include "messages/RPC/requestVote.hh"
#include "messages/RPC/vote.hh"
#include "messages/mpi_wrappers.hh"
#include "messages/status.hh"
#include "server.h"
#include "spdlog/spdlog.h"

using namespace MessageNS;

void Server::handleRequestVote(const json &json) {
  RPC::RequestVote request(json);
  spdlog::info("{}: Received request vote from {}", id, request.getCandidate());
  checkTerm(request.getTerm());

  bool grantVote = false;

  if (request.getTerm() >= this->term &&
      (this->voted_for == -1 || this->voted_for == request.getCandidate())) {
    grantVote = true;
    this->voted_for = request.getCandidate();

    // Reset election timeout
    start_time = std::chrono::system_clock::now();
  }

  RPC::Vote vote(this->term, grantVote, this->id);
  send(vote, request.getCandidate());
}

void Server::handleVote(const json &json) {
  RPC::Vote vote(json);
  checkTerm(vote.getTerm());

  // Check state after checkTerm
  if (state != CANDIDATE) {
    spdlog::info("{}: Received vote but not candidate", id);
    return;
  }

  if (vote.isGranted()) {
    this->vote_count++;
    spdlog::info("{}: Vote granted by {}", id, vote.getOriginId());
  } else {
    spdlog::info("{}: Vote denied by {}", id, vote.getOriginId());
  }
}

void Server::handleAppendEntries(const json &json) {
  RPC::AppendEntries appendEntry(json);
  spdlog::info("{}: Received append entries from {}", id,
               appendEntry.getLeader());

  // TODO Create response

  // Reply false if term < currentTerm
  if (appendEntry.getTerm() < this->term) {
    // TODO Replace status by AppendEntriesResponse
    Status status(this->term, false, this->id);
    return send(status, appendEntry.getLeader());
  }

  // TODO Check if log is consistent
  // if (log.isHeartbeat()) {
  //   spdlog::info("{}: Received heartbeat from {}", id, log.getLeader());
  //   this->start_time = std::chrono::system_clock::now();
  // } else

  // Reply false if log doesn't contain an entry at prevLogIndex whose term
  // matches prevLogTerm
  if (!m_logs.contains(appendEntry.getPreviousLogIdx()) ||
      m_logs.getTerm(appendEntry.getPreviousLogIdx()) != appendEntry.getPreviousLogTerm()) {
    // response.success = false;
  }
  // If an existing entry conflicts with a new one (same index but different
  // terms), delete the existing entry and all that follow it
  if (m_logs.contains(appendEntry.getPreviousLogIdx()) &&
      m_logs.getTerm(appendEntry.getPreviousLogIdx()) != appendEntry.getPreviousLogTerm()) {
    m_logs.deleteLastLogs(appendEntry.getPreviousLogIdx());
  }
  // Return ?

  // Append any new entries not already in the log
  m_logs.addLogs(appendEntry.getEntries());

  // If leaderCommit > commitIndex, set commitIndex = min(leaderCommit,
  // index of last new entry)
  m_logs.updateCommitIndex(appendEntry.getLeaderCommit());
  // if (appendEntry.getLeaderCommit() > m_logs.getCommitIndex())
  //   m_logs.setCommitIndex(min(leaderCommit, m_logs.getLastLogs()));
}

void Server::handleAppendEntriesResponse(const json &json) {
  RPC::AppendEntriesResponse response(json);
  checkTerm(response.getTerm());

  if (state != LEADER) {
    spdlog::info("{}: Received append entries response but not leader", id);
    return;
  }

  spdlog::info("{}: Received append entries response from {}", id,
               response.getOriginId());

  // Update nextIndex and matchIndex for follower
  if (response.isSuccess()) {
    // If successful: update nextIndex and matchIndex for follower
    m_nextIndex[response.getOriginId()] = response.getMatchIndex() + 1;
    m_matchIndex[response.getOriginId()] = response.getMatchIndex();
  } else {
    // If AppendEntries fails because of log inconsistency: decrement nextIndex
    // and retry
    m_nextIndex[response.getOriginId()] =
        std::min(m_nextIndex[response.getOriginId()] - 1, 1);
  }

  // If there exists an N such that N > commitIndex, a majority of
  // matchIndex[i] >= N, and log[N].term == currentTerm: set commitIndex = N
  int N = m_logs.getCommitIndex() + 1;
  int count = std::count(m_matchIndex.begin(), m_matchIndex.end(), N);
  while (N <= m_logs.getLastIndex() && count > world_size / 2 &&
         m_logs.getTerm(N) == term) {
    N++;
    count = std::count(m_matchIndex.begin(), m_matchIndex.end(), N);
  }
  m_logs.updateCommitIndex(N - 1);
}