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

  bool grantVote = false;

  if (request.getTerm() > this->term &&
      (this->voted_for == -1 || this->voted_for == request.getCandidate())) {
    grantVote = true;
    this->voted_for = request.getCandidate();
  }

  RPC::Vote vote(this->term, grantVote, this->id);
  send(vote, request.getCandidate());
}

void Server::handleVote(const json &json) {
  RPC::Vote vote(json);

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
  spdlog::info("{}: Received append entries from {}", id, log.getLeader());

  if (log.getTerm() < this->term) {
    Status status(this->term, false, this->id);
    send(status, log.getLeader());
  }

  if (log.isHeartbeat()) {
    this->start_time = std::chrono::system_clock::now();
  } else {
    // TODO check if log contains an entry at prevLogIndex whose term matches
    // prevLogTerm

    // TODO check if entry conflicts with a new one (same index but different
    // terms)

    // TODO append new entries not already in the log

    // if (log.getLeaderCommit() > this->commitIndex)
    // this->commitIndex = min(log.getLeaderCommit(), index of last new entry)
  }
}