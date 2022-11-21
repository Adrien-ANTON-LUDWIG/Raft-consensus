#include "server.hh"

#include "messages/CMD/loadResponse.hh"
#include "messages/REPL/info.hh"
#include "messages/RPC/appendEntries.hh"
#include "messages/RPC/requestVote.hh"
#include "messages/RPC/vote.hh"
#include "messages/mpi_wrappers.hh"
#include "messages/redirect.hh"
#include "spdlog/spdlog.h"

using namespace MessageNS;

Server::Server(Universe universe, int replRank) : ::REPL::Process(replRank) {
  m_universe = universe;

  std::srand(universe.replWorld.rank);
  m_election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
  m_heartbeat_timeout = std::chrono::milliseconds(50);
  m_start_time = std::chrono::system_clock::now();

  m_nextIndex = std::vector<int>(universe.replWorld.world_size, 1);
  m_matchIndex = std::vector<int>(universe.replWorld.world_size, 0);

  spdlog::debug("{}: Election timeout: {}", universe.serverWorld.rank,
                m_election_timeout.count());
}

void Server::checkREPL() {
  std::optional<MPI_Status> statusOpt =
      checkForMessage(m_universe.replWorld.com, m_replRank);
  if (statusOpt.has_value()) {
    json query = recv(statusOpt.value(), m_universe.replWorld.com);
    auto type = Message::getType(query);
    if (type == Message::Type::REPL_INFO)
      handleREPLInfo(query);
    else if (type == Message::Type::REPL_CRASH)
      handleREPLCrash(query);
    else if (type == Message::Type::REPL_SPEED) {
      handleREPLSpeed(query);
    } else if (type == Message::Type::REPL_STOP) {
      m_logs.writeLogs(m_universe.replWorld.rank);
      m_isRunning = false;
    }
  }
}

void Server::run() {
  m_isRunning = true;
  spdlog::info("Server {} started.", m_universe.replWorld.rank);

  while (m_isRunning) {
    checkREPL();
    if (!m_isRunning) break;

    if (m_isCrashed) continue;

    if (std::chrono::duration<float, std::milli>(
            std::chrono::system_clock::now() - m_speedCheckpoint) <
        std::chrono::milliseconds(m_speed))
      continue;

    // Update the current_time
    m_current_time = std::chrono::system_clock::now();

    // Apply committed logs
    m_logs.apply();

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

  spdlog::info("Server {} stopped with {} logs.", m_universe.replWorld.rank, m_logs.getLastIndex());
}

// FOLLOWER
void Server::becomeFollower() {
  if (m_state != FOLLOWER) {
    spdlog::debug("{}: Become follower", m_universe.serverWorld.rank);

    // Update state
    m_state = FOLLOWER;
  }

  // Reset vote attributes
  m_voted_for = -1;
  m_vote_count = 0;
}

void Server::followerUpdate() {
  // Respond to RPCs from candidates and leaders
  std::optional<MPI_Status> status_server =
      checkForMessage(m_universe.serverWorld.com);

  if (status_server.has_value()) {
    json data = recv(*status_server, m_universe.serverWorld.com);
    if (status_server->MPI_TAG == Message::Type::RPC_REQUEST_VOTE)
      handleRequestVote(data);
    else if (status_server->MPI_TAG == Message::Type::RPC_APPEND_ENTRIES)
      handleAppendEntries(data);
    else {  // non expected or invalid message -> drop
      dropMessage(data);
    }
  }

  // Redirect clients to leader if received message
  std::optional<MPI_Status> status_client =
      checkForMessage(m_universe.clientServerWorld.com);

  if (status_client.has_value()) {
    json data = recv(*status_client, m_universe.clientServerWorld.com);
    if (Message::isCMD(status_client->MPI_TAG)) {
      Redirect redirection(m_leaderId, m_universe.clientServerWorld.rank);
      send(redirection, status_client->MPI_SOURCE,
           m_universe.clientServerWorld.com);
    }
    dropMessage(data);
  }

  // Election timeout : convert to candidate
  if (m_current_time - m_start_time >= m_election_timeout)
    return becomeCandidate();
}

// ELECTION
void Server::becomeCandidate() {
  // START ELECTION
  spdlog::debug("{}: Become candidate", m_universe.serverWorld.rank);

  // Update state
  m_state = STATE::CANDIDATE;

  // Increment current term
  m_term++;

  // Vote for self
  m_voted_for = m_universe.serverWorld.rank;
  m_vote_count = 1;

  // Reset election timer
  m_start_time = std::chrono::system_clock::now();
  m_election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);

  // Send RequestVote RPCs to all other servers
  RPC::RequestVote requestVote(m_term, m_universe.serverWorld.rank);
  sendAll(requestVote, m_universe.serverWorld.rank,
          m_universe.serverWorld.world_size, m_universe.serverWorld.com);
}

void Server::candidateUpdate() {
  // If votes received from majority of servers: become leader
  if (m_vote_count > m_universe.serverWorld.world_size / 2) becomeLeader();

  // If AppendEntries RPC received from new leader: convert to follower
  std::optional<MPI_Status> status_server =
      checkForMessage(m_universe.serverWorld.com);

  if (status_server.has_value()) {
    json data = recv(*status_server, m_universe.serverWorld.com);
    if (status_server->MPI_TAG == Message::Type::RPC_VOTE)
      handleVote(data);
    else if (status_server->MPI_TAG == Message::Type::RPC_APPEND_ENTRIES)
      handleAppendEntries(data);
    else {  // non expected or invalid message -> drop
      dropMessage(data);
    }
  }

  // Redirect clients to leader if received message
  std::optional<MPI_Status> status_client =
      checkForMessage(m_universe.clientServerWorld.com);

  if (status_client.has_value()) {
    if (Message::isCMD(status_client->MPI_TAG)) {
      Redirect redirection(-1, m_universe.clientServerWorld.rank);
      send(redirection, status_client->MPI_SOURCE,
           m_universe.clientServerWorld.com);
    }
    dropMessage(recv(*status_client, m_universe.clientServerWorld.com));
  }

  // If election timeout elapses: start new election
  if (m_current_time - m_start_time >= m_election_timeout) becomeCandidate();
}

// LEADER
void Server::becomeLeader() {
  spdlog::debug("{}: Become leader", m_universe.serverWorld.rank);

  // Update state
  m_state = STATE::LEADER;

  // Set nextIndex for each server to lastLogIndex + 1
  m_nextIndex = std::vector<int>(m_universe.serverWorld.world_size,
                                 m_logs.getLastIndex() + 1);

  // Set matchIndex for each server to 0 (initialized, increases monotonically)
  m_matchIndex = std::vector<int>(m_universe.serverWorld.world_size, 0);

  // Upon election: send heartbeat to each server
  sendHeartbeat();
}

void Server::leaderUpdate() {
  // Check for new messages
  std::optional<MPI_Status> status =
      checkForMessage(m_universe.serverWorld.com);

  if (status.has_value()) {
    json data = recv(*status, m_universe.serverWorld.com);
    if (status->MPI_TAG == Message::Type::RPC_REQUEST_VOTE)
      handleRequestVote(data);
    else if (status->MPI_TAG == Message::Type::RPC_APPEND_ENTRIES)
      handleAppendEntries(data);
    else if (status->MPI_TAG == Message::Type::RPC_APPEND_ENTRIES_RESPONSE)
      handleAppendEntriesResponse(data);
  }

  status = checkForMessage(m_universe.clientServerWorld.com);
  if (status.has_value()) {
    // If command received from client: append entry to local log,
    // respond after entry applied to state machine (§5.3)

    json data = recv(*status, m_universe.clientServerWorld.com);
    if (Message::isCMD(status->MPI_TAG)) {
      m_logs.addLog(m_term, data);
      if (status->MPI_TAG == Message::Type::CMD_LOAD) handleLoad(data);
    } else  // non expected or invalid message -> drop
      dropMessage(data);
  }

  // Send appendEntries RPCs to each follower
  for (int rank = 0; rank < m_universe.serverWorld.world_size; rank++) {
      if (rank == m_universe.serverWorld.rank) continue;

      int prevLogIndex = m_nextIndex[rank] - 1;
      int prevLogTerm = -1;
      if (prevLogIndex > 0) m_logs.getLog(prevLogIndex).getTerm();

      auto appendEntries = RPC::AppendEntries::createHeartbeat(
          m_term, m_universe.serverWorld.rank, prevLogIndex, prevLogTerm,
          m_logs.getCommitIndex());

      // If last log index ≥ nextIndex for a follower: send
      // AppendEntries RPC with log entries starting at nextIndex
      if (m_logs.getLastIndex() >= m_nextIndex[rank]) {
        appendEntries.setEntries(m_logs.getLastLogs(m_nextIndex[rank]));
        send(appendEntries, rank, m_universe.serverWorld.com);
      } else if (m_current_time - m_start_time >= m_heartbeat_timeout) {
        // Idle time -> send heartbeat
        send(appendEntries, rank, m_universe.serverWorld.com);
      }
  }

  // Reset heartbeat timer
  m_start_time = std::chrono::system_clock::now();
}

void Server::sendHeartbeat() {
  for (int rank = 0; rank < m_universe.serverWorld.world_size; rank++) {
    if (rank == m_universe.serverWorld.rank) continue;
    int prevLogIndex = m_nextIndex[rank] - 1;
    int prevLogTerm = -1;
    if (prevLogIndex > 0) m_logs.getLog(prevLogIndex).getTerm();
    auto heartbeat = RPC::AppendEntries::createHeartbeat(
        m_term, m_universe.serverWorld.rank, prevLogIndex, prevLogTerm,
        m_logs.getCommitIndex());
    send(heartbeat, rank, m_universe.serverWorld.com);
  }

  // Reset heartbeat timer
  m_start_time = std::chrono::system_clock::now();
}

// UTILS
void Server::dropMessage(const Message &message) {
  spdlog::debug("{}: Dropping message {}", m_universe.serverWorld.rank,
                message.toJSON().dump());
}

void Server::checkTerm(int term) {
  if (term > m_term) {
    m_term = term;
    becomeFollower();
  }
}
