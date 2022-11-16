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
  //     • If successful: update nextIndex and matchIndex for
  //     follower (§5.3)

  //     • If AppendEntries fails because of log inconsistency:
  //     decrement nextIndex and retry (§5.3)

  // If there exists an N such that N > commitIndex, a majority
  // of matchIndex[i] ≥ N, and log[N].term == currentTerm:
  // set commitIndex = N (§5.3, §5.4)

  // Send heartbeat to each server : repeat during idle periods to prevent
  // election timeouts
}