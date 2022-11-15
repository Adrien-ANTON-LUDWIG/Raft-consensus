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
  }
  else {
    spdlog::info("{}: Vote denied by {}", id, vote.getOriginId());
  }
}

void Server::handleAppendEntries(const json &json) {
  RPC::AppendEntries log(json);

  if (log.getTerm() < this->term) {
    Status status(this->term, false, this->id);
    return send(status, log.getLeader());
  }

  if (log.isHeartbeat()) {
    spdlog::info("{}: Received heartbeat from {}", id, log.getLeader());
    this->start_time = std::chrono::system_clock::now();
  } else {
    spdlog::info("{}: Received append entries from {}", id, log.getLeader());
    // TODO check if log contains an entry at prevLogIndex whose term matches
    // prevLogTerm

    // TODO check if entry conflicts with a new one (same index but different
    // terms)

    // TODO append new entries not already in the log

    // if (log.getLeaderCommit() > this->commitIndex)
    // this->commitIndex = min(log.getLeaderCommit(), index of last new entry)
  }
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