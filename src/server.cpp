#include "server.h"

#include "messages/RPC/appendEntries.hh"
#include "messages/RPC/requestVote.hh"
#include "messages/RPC/vote.hh"
#include "messages/mpi_wrappers.hh"
#include "spdlog/spdlog.h"

using namespace MessageNS;

Server::Server(int id, int world_size) {
  this->id = id;
  this->world_size = world_size;

  std::srand(id);
  election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
  heartbeat_timeout = std::chrono::milliseconds(50);
  start_time = std::chrono::system_clock::now();

  m_nextIndex = std::vector<int>(world_size, 1);
  m_matchIndex = std::vector<int>(world_size, 0);

  spdlog::info("{}: Election timeout: {}", id, election_timeout.count());
}

// GENERAL UPDATE
void Server::update() {
  // Update the current_time
  current_time = std::chrono::system_clock::now();

  // Apply committed logs
  m_logs.apply();

  switch (state) {
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
  if (state != FOLLOWER) {
    spdlog::info("{}: Become follower", id);

    // Update state
    state = FOLLOWER;
  }

  // Reset vote attributes
  voted_for = -1;
  vote_count = 0;
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
  if (current_time - start_time >= election_timeout) return becomeCandidate();
}

// ELECTION
void Server::becomeCandidate() {
  // START ELECTION
  spdlog::info("{}: Become candidate", id);

  // Update state
  state = STATE::CANDIDATE;

  // Increment current term
  term++;

  // Vote for self
  voted_for = id;
  vote_count = 1;

  // Reset election timer
  start_time = std::chrono::system_clock::now();
  election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);

  // Send RequestVote RPCs to all other servers
  RPC::RequestVote requestVote(term, id);
  sendAll(requestVote, id, world_size);
}

void Server::candidateUpdate() {
  // If votes received from majority of servers: become leader
  if (vote_count > world_size / 2) becomeLeader();

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
  if (current_time - start_time >= election_timeout) becomeCandidate();
}

// LEADER
void Server::becomeLeader() {
  spdlog::info("{}: Become leader", id);

  // Update state
  state = STATE::LEADER;

  // Set nextIndex for each server to lastLogIndex + 1
  m_nextIndex = std::vector<int>(world_size, m_logs.getLastIndex() + 1);

  // Set matchIndex for each server to 0 (initialized, increases monotonically)
  m_matchIndex = std::vector<int>(world_size, 0);

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

    // If command received from client: append entry to local log,
    // respond after entry applied to state machine (§5.3)
    else if (Message::isCMD(status->MPI_TAG))
      m_logs.addLog(term, recv(*status));

    else  // non expected or invalid message -> drop
      dropMessage(recv(*status));
  }

  // Send appendEntries RPCs to each follower
  if (current_time - start_time >= heartbeat_timeout) {
    for (int rank = 0; rank < world_size; rank++) {
      if (rank == id) continue;

      int prevLogIndex = m_nextIndex[rank] - 1;
      int prevLogTerm = -1;
      if (prevLogIndex > 0) m_logs.getLog(prevLogIndex).getTerm();

      RPC::AppendEntries appendEntries(term, id, prevLogIndex, prevLogIndex,
                                       m_logs.getCommitIndex());

      // If last log index ≥ nextIndex for a follower: send
      // AppendEntries RPC with log entries starting at nextIndex
      if (m_logs.getLastIndex() >= m_nextIndex[rank])
        appendEntries.setEntries(m_logs.getLastLogs(m_nextIndex[rank]));

      send(appendEntries, rank);
    }

    // Reset heartbeat timer
    start_time = std::chrono::system_clock::now();
  }
}

void Server::sendHeartbeat() {
  for (int rank = 0; rank < world_size; rank++) {
    if (rank == id) continue;
    int prevLogIndex = m_nextIndex[rank] - 1;
    int prevLogTerm = -1;
    if (prevLogIndex > 0) m_logs.getLog(prevLogIndex).getTerm();
    RPC::AppendEntries heartbeat(term, id, prevLogIndex, prevLogTerm,
                                 m_logs.getCommitIndex());
    send(heartbeat, rank);
  }

  // Reset heartbeat timer
  start_time = std::chrono::system_clock::now();
}

// UTILS
void Server::dropMessage(const Message &message) {
  spdlog::info("{}: Dropping message {}", id, message.toJSON().dump());
}

void Server::checkTerm(int term) {
  if (term > this->term) {
    this->term = term;
    becomeFollower();
  }
}
