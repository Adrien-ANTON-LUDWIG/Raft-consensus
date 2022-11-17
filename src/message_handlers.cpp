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
  checkTerm(appendEntry.getTerm());

  spdlog::info("{}: Received append entries from {}", id,
               appendEntry.getLeader());

  // Create response
  RPC::AppendEntriesResponse response(term, false, id);

  bool logOk = appendEntry.getPreviousLogIdx() == 0 ||
               (m_logs.contains(appendEntry.getPreviousLogIdx()) &&
                appendEntry.getPreviousLogTerm() ==
                    m_logs.getTerm(appendEntry.getPreviousLogIdx()));

  // Reply false if term < currentTerm or if log doesn't contain an entry at
  // prevLogIndex whose term matches prevLogTerm
  if (appendEntry.getTerm() < this->term || (state == FOLLOWER && !logOk))
    return send(response, appendEntry.getLeader());

  if (state == CANDIDATE && appendEntry.getTerm() >= term) becomeFollower();

  // Accept AppendEntries
  response.setSuccess(true);

  // Reset election timeout
  start_time = std::chrono::system_clock::now();
  m_leaderId = appendEntry.getLeader();

  int index = appendEntry.getPreviousLogIdx() + 1;

  // Nothing to do
  if (appendEntry.isHeartbeat()) {
    m_logs.updateCommitIndex(appendEntry.getLeaderCommit());
    response.setMatchIndex(m_logs.getLastIndex());
    return send(response, appendEntry.getLeader());
  }

  // Delete all entries after previous log index
  m_logs.deleteLastLogs(appendEntry.getPreviousLogIdx());

  // Append any new entries not already in the log
  m_logs.addLogs(appendEntry.getEntries());

  // If leaderCommit > commitIndex, set commitIndex = min(leaderCommit,
  // index of last new entry)
  m_logs.updateCommitIndex(appendEntry.getLeaderCommit());

  response.setMatchIndex(m_logs.getLastIndex());
  send(response, appendEntry.getLeader());
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