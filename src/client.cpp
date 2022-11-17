#include "client.hh"

#include <fstream>
#include <sstream>
#include <string>

#include "messages/CMD/load.hh"
#include "messages/mpi_wrappers.hh"

using namespace MessageNS;

Client::Client(int id, int nbServer) : m_id(id), m_nbServer(nbServer) {}

void Client::update() {
  // Update time
  m_currentTime = std::chrono::system_clock::now();

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
      } else if (client_rank == m_id) {
        if (line[0] == '$') {
          // TODO specials instructions
          std::string cmd = line.substr(line.find(' '));
        } else if (line[0] != '#') {
          size_t commentStart = line.find('#');
          if (commentStart != std::string::npos)
            line = line.substr(0, commentStart);

          Message command = parseCommand(line);
          m_commands.push_back(command);
        }
      }
    }
  }
}

Message Client::parseCommand(const std::string& command) {
  std::istringstream iss(command);
  std::vector<std::string> tokens;
  std::string t;

  while (iss >> t && tokens.size() < 3) tokens.push_back(t);
  getline(iss, t);
  tokens.push_back(t);

  auto& cmd = tokens[0];
  cmd == "LOAD";

  if (cmd == "LOAD") {
    if (tokens.size() < 2)
      throw std::invalid_argument(
          "Missing filename. Usage: LOAD <file_name>\n");
    else if (tokens.size() > 2)
      throw std::invalid_argument(
          "Too many arguments. Usage: LOAD <file_name>\n");

    std::string file_name = tokens[1];
    return CMD::Load(file_name, m_id);
  } else if (cmd == "LIST") {
    if (tokens.size() != 1)
      throw std::invalid_argument("Too many arguments. Usage: LIST\n");

    // return CMD::List(m_id);
    throw std::logic_error("Not implemented yet\n");
  } else if (cmd == "DELETE") {
    if (tokens.size() < 2)
      throw std::invalid_argument(
          "Missing file index. Usage: DELETE <file_index>\n");
    else if (tokens.size() > 2)
      throw std::invalid_argument(
          "Too many arguments. Usage: DELETE <file_index>\n");

    int file_index = std::stoi(tokens[1]);
    // return CMD::Delete(file_index, m_id);
    throw std::logic_error("Not implemented yet\n");
  } else if (cmd == "APPEND") {
    if (tokens.size() < 3)
      throw std::invalid_argument(
          "Missing file index and/or data. Usage: APPEND <file_index> "
          "<data>\n");

    int file_index = std::stoi(tokens[1]);
    std::string data = tokens[2];
    // return CMD::Append(file_index, data, m_id);
    throw std::logic_error("Not implemented yet\n");
  } else {
    throw std::invalid_argument("Unknown command");
  }
}
