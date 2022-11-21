#include "messages/RPC/appendEntries.hh"
#include "messages/RPC/appendEntriesResponse.hh"
#include "messages/RPC/requestVote.hh"
#include "messages/RPC/vote.hh"
#include "messages/mpi_wrappers.hh"
#include "messages/redirect.hh"
#include "server.hh"
#include "spdlog/spdlog.h"

using namespace MessageNS;

void Server::handleRequestVote(const json &json)
{
  RPC::RequestVote request(json);
  spdlog::debug("{}: Received request vote from {}", m_universe.serverWorld.rank,
                request.getCandidate());
  checkTerm(request.getTerm());

  bool grantVote = false;

  if (request.getTerm() >= m_term &&
      (m_voted_for == -1 || m_voted_for == request.getCandidate()))
  {
    grantVote = true;
    m_voted_for = request.getCandidate();

    // Reset election timeout
    m_start_time = std::chrono::system_clock::now();
  }

  RPC::Vote vote(m_term, grantVote, m_universe.serverWorld.rank);
  send(vote, request.getCandidate(), m_universe.serverWorld.com);
}

void Server::handleVote(const json &json)
{
  RPC::Vote vote(json);
  checkTerm(vote.getTerm());

  // Check state after checkTerm
  if (m_state != CANDIDATE)
  {
    spdlog::debug("{}: Received vote but not candidate", m_universe.serverWorld.rank);
    return;
  }

  if (vote.isGranted())
  {
    m_vote_count++;
    spdlog::debug("{}: Vote granted by {}", m_universe.serverWorld.rank, vote.getOriginId());
  }
  else
  {
    spdlog::debug("{}: Vote denied by {}", m_universe.serverWorld.rank, vote.getOriginId());
  }
}

void Server::handleAppendEntries(const json &json)
{
  RPC::AppendEntries appendEntry(json);
  checkTerm(appendEntry.getTerm());

  spdlog::debug("{}: Received append entries from {}", m_universe.serverWorld.rank,
                appendEntry.getLeader());

  // Create response
  RPC::AppendEntriesResponse response(m_term, false, m_universe.serverWorld.rank);

  bool logOk = appendEntry.getPreviousLogIdx() == 0 ||
               (m_logs.contains(appendEntry.getPreviousLogIdx()) &&
                appendEntry.getPreviousLogTerm() ==
                    m_logs.getTerm(appendEntry.getPreviousLogIdx()));

  // Reply false if term < currentTerm or if log doesn't contain an entry at
  // prevLogIndex whose term matches prevLogTerm
  if (appendEntry.getTerm() < m_term || (m_state == FOLLOWER && !logOk))
    return send(response, appendEntry.getLeader(), m_universe.serverWorld.com);

  if (m_state == CANDIDATE && appendEntry.getTerm() >= m_term)
    becomeFollower();

  // Accept AppendEntries
  response.setSuccess(true);

  // Reset election timeout
  m_start_time = std::chrono::system_clock::now();
  m_leaderId = appendEntry.getLeader();

  // Nothing to do
  if (appendEntry.isHeartbeat())
  {
    m_logs.updateCommitIndex(appendEntry.getLeaderCommit());
    response.setMatchIndex(m_logs.getLastIndex());
    return send(response, appendEntry.getLeader(), m_universe.serverWorld.com);
  }

  // Delete all entries after previous log index
  if (m_logs.getLastIndex() > 0)
    m_logs.deleteLastLogs(appendEntry.getPreviousLogIdx() + 1);

  // Append any new entries not already in the log
  m_logs.addLogs(appendEntry.getEntries());

  // If leaderCommit > commitIndex, set commitIndex = min(leaderCommit,
  // index of last new entry)
  m_logs.updateCommitIndex(appendEntry.getLeaderCommit());

  response.setMatchIndex(m_logs.getLastIndex());
  send(response, appendEntry.getLeader(), m_universe.serverWorld.com);
}

void Server::handleAppendEntriesResponse(const json &json)
{
  RPC::AppendEntriesResponse response(json);
  checkTerm(response.getTerm());

  if (m_state != LEADER)
  {
    spdlog::debug("{}: Received append entries response but not leader", m_universe.serverWorld.rank);
    return;
  }

  spdlog::debug("{}: Received append entries response from {}", m_universe.serverWorld.rank,
                response.getOriginId());

  // Update nextIndex and matchIndex for follower
  if (response.isSuccess())
  {
    // If successful: update nextIndex and matchIndex for follower
    m_nextIndex[response.getOriginId()] = response.getMatchIndex() + 1;
    m_matchIndex[response.getOriginId()] = response.getMatchIndex();
  }
  else
  {
    // If AppendEntries fails because of log inconsistency: decrement nextIndex
    // and retry
    m_nextIndex[response.getOriginId()] =
        std::min(m_nextIndex[response.getOriginId()] - 1, 1);
  }

  // If there exists an N such that N > commitIndex, a majority of
  // matchIndex[i] >= N, and log[N].term == currentTerm: set commitIndex = N
  int N = m_logs.getCommitIndex() + 1;
  int count = std::count(m_matchIndex.begin(), m_matchIndex.end(), N);
  while (N <= m_logs.getLastIndex() && count > m_universe.serverWorld.world_size / 2 &&
         m_logs.getTerm(N) == m_term)
  {
    N++;
    count = std::count(m_matchIndex.begin(), m_matchIndex.end(), N);
  }
  m_logs.updateCommitIndex(N - 1);
}