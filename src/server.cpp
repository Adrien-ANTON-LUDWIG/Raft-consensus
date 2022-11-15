#include "server.h"

#include "messages/RPC/appendEntries.hh"
#include "messages/RPC/requestVote.hh"
#include "messages/RPC/vote.hh"
#include "messages/mpi_wrappers.hh"
#include "spdlog/spdlog.h"

// TODO
#define TO_IMPLEMENT 42

using namespace MessageNS;

Server::Server(int id, int world_size) {
  m_id = id;
  m_world_size = world_size;

  std::srand(id);
  m_election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
  m_heartbeat_timeout = std::chrono::milliseconds(50);
  m_start_time = std::chrono::system_clock::now();

  spdlog::info("{}: Election timeout: {}", m_id, m_election_timeout.count());
}

// GENERAL UPDATE
void Server::update() {
  // Update the current_time
  m_current_time = std::chrono::system_clock::now();

  // TODO
  // If commitIndex > lastApplied: increment lastApplied, apply log[lastApplied]
  // to state machine

  switch (m_state) {
    case LEADER:
      leaderUpdate();
      break;
    case CANDIDATE:
      candidateUpdate();
      break;
    default:  // FOLLOWER
      followerUpdate();
      break;
  }
}

// FOLLOWER
void Server::becomeFollower() { 

  if (m_state != FOLLOWER) {
    spdlog::info("{}: Become follower", m_id);

    // Update state
    m_state = FOLLOWER;
  }

  // Reset vote attributes
  m_voted_for = -1;
  m_vote_count = 0;
}

void Server::followerUpdate() {
  // Respond to RPCs from candidates and leaders
  std::optional<MPI_Status> status = checkForMessage();

  if (status.has_value()) {
    if (status->MPI_TAG == Message::RPC_REQUEST_VOTE)
      handleRequestVote(recv(*status));
    else if (status->MPI_TAG == Message::Message::RPC_APPEND_ENTRIES)
      handleAppendEntries(recv(*status));
    // else if CMD from client -> reject and redirect to leader
    else  // non expected or invalid message -> drop
      dropMessage(recv(*status));
  }

  // Election timeout : convert to candidate
  if (m_current_time - m_start_time >= m_election_timeout) return becomeCandidate();
}

// ELECTION
void Server::becomeCandidate() {
  // START ELECTION
  spdlog::info("{}: Become candidate", m_id);

  // Update state
  m_state = STATE::CANDIDATE;

  // Increment current term
  m_term++;

  // Vote for self
  m_voted_for = m_id;
  m_vote_count = 1;

  // Reset election timer
  m_start_time = std::chrono::system_clock::now();
  m_election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);

  // Send RequestVote RPCs to all other servers
  RPC::RequestVote requestVote(m_term, m_id);
  sendAll(requestVote, m_id, m_world_size);
}

void Server::candidateUpdate() {
  // If votes received from majority of servers: become leader
  if (m_vote_count > m_world_size / 2) becomeLeader();

  // If AppendEntries RPC received from new leader: convert to follower
  std::optional<MPI_Status> status = checkForMessage();

  if (status.has_value()) {
    if (status->MPI_TAG == Message::RPC_VOTE)
      handleVote(recv(*status));
    else if (status->MPI_TAG == Message::RPC_APPEND_ENTRIES)
      handleAppendEntries(recv(*status));
    else  // non expected or invalid message -> drop
      dropMessage(recv(*status));
  }

  // • If election timeout elapses: start new election
  if (m_current_time - m_start_time >= m_election_timeout) becomeCandidate();
}

// LEADER
void Server::becomeLeader() {
  spdlog::info("{}: Become leader", m_id);

  // Update state
  m_state = STATE::LEADER;

  // Upon election: send heartbeat to each server
  sendHeartbeat();
}

void Server::leaderUpdate() {

  // Check for new messages
  std::optional<MPI_Status> status = checkForMessage();

  if (status.has_value()) {
    if (status->MPI_TAG == Message::RPC_REQUEST_VOTE)
      handleRequestVote(recv(*status));
    else if (status->MPI_TAG == Message::RPC_APPEND_ENTRIES)
      handleAppendEntries(recv(*status));
    else  // non expected or invalid message -> drop
      dropMessage(recv(*status));
  }

  // If command received from client: append entry to local log,
  // respond after entry applied to state machine (§5.3)

  // If last log index ≥ nextIndex for a follower: send
  // AppendEntries RPC with log entries starting at nextIndex
  //     • If successful: update nextIndex and matchIndex for
  //     follower (§5.3)

  //     • If AppendEntries fails because of log inconsistency:
  //     decrement nextIndex and retry (§5.3)

  // If there exists an N such that N > commitIndex, a majority
  // of matchIndex[i] ≥ N, and log[N].term == currentTerm:
  // set commitIndex = N (§5.3, §5.4)

  // Send heartbeat to each server : repeat during idle periods to prevent
  // election timeouts
  if (m_current_time - m_start_time >= m_heartbeat_timeout) sendHeartbeat();
}

void Server::sendHeartbeat() {
  RPC::AppendEntries heartbeat(m_term, m_id, TO_IMPLEMENT, TO_IMPLEMENT,
                                        TO_IMPLEMENT);
  sendAll(heartbeat, m_id, m_world_size);
}

// UTILS
void Server::dropMessage(const Message &message) {
  spdlog::info("{}: Dropping message {}", m_id, message.toJSON().dump());
}

void Server::checkTerm(int term) {
  if (term > m_term) {
    m_term = term;
    becomeFollower();
  }
}
