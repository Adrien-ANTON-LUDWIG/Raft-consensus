#include "client.hh"

#include <fstream>

#include "messages/CMD/load.hh"
#include "messages/mpi_wrappers.hh"
#include "messages/REPL/info.hh"

using namespace MessageNS;

Client::Client(int id, int nbServer, int replRank) : ::REPL::Process(replRank)  {
  m_id = id;
  m_nbServer = nbServer;
}

void Client::update() {
  std::optional<MPI_Status> statusOpt = checkForMessage(m_replRank);
  if (statusOpt.has_value()) {
    json query = recv(statusOpt.value());
    auto type = Message::getType(query);
    if (type == Message::Type::REPL_INFO)
    {
      MessageNS::REPL::InfoResponse response(m_speed, m_isCrashed, true, m_isStarted, m_id);
      send(response, m_replRank);
    }
  }

  // Create message
  CMD::Load message("test.txt", m_id);

  // Send message
  send(message, m_leaderId);

  // Receive response
  std::optional<MPI_Status> status = checkForMessage();

  if (!status.has_value()) return;

  // if (!response.success) {
    // Look leaderId
    // Save it
    // Send message to the leader
  // }
}

void Client::loadCommands(const std::string& path) {
  std::ifstream stream(path);
  if (stream.is_open()) {
    std::string line;
    int client_rank = -1;
    while (std::getline(stream, line)) {
      if (line[0] == '-') {
        client_rank++;
      }
      else if (client_rank == m_id)
      {
        if (line[0] == '$') {
          // TODO specials instructions
          std::string cmd = line.substr(line.find(' '));
        }
        else if (line[0] != '#') {
          size_t commentStart = line.find('#');
          if (commentStart != std::string::npos)
            line = line.substr(0, commentStart);

          m_commands.push_back(line);
        }
      }
    }
  }
}