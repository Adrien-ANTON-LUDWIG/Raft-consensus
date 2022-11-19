#include "client.hh"

#include <fstream>
#include <sstream>
#include <string>

#include "messages/CMD/append.hh"
#include "messages/CMD/appendResponse.hh"
#include "messages/CMD/delete.hh"
#include "messages/CMD/deleteResponse.hh"
#include "messages/CMD/list.hh"
#include "messages/CMD/listResponse.hh"
#include "messages/CMD/load.hh"
#include "messages/CMD/loadResponse.hh"
#include "messages/mpi_wrappers.hh"
#include "messages/REPL/info.hh"
#include "spdlog/spdlog.h"

using namespace MessageNS;

Client::Client(int id, int nbServer, int replRank) : ::REPL::Process(replRank)  {
  m_id = id;
  m_nbServer = nbServer;

  // Time management
  m_requestTimeout = std::chrono::milliseconds(std::rand() % 1000 + 1000);
  m_startTime = std::chrono::system_clock::now();
  m_currentTime = std::chrono::system_clock::now();

  // REPL
  m_isCrashed = false;
  m_isStarted = false;
}

void Client::update() {
  std::optional<MPI_Status> statusOpt = checkForMessage(m_replRank);
  if (statusOpt.has_value()) {
    json query = recv(statusOpt.value());
    auto type = Message::getType(query);
    if (type == Message::Type::REPL_INFO)
      handleREPLInfo(query);
    else if (type == Message::Type::REPL_START)
      handleREPLStart(query);
    else if (type == Message::Type::REPL_CRASH)
      handleREPLCrash(query);
    else if (type == Message::Type::REPL_SPEED)
      handleREPLSpeed(query);
    else if (type == Message::Type::REPL_STOP)
      exit(0);
  }

  if (!m_isStarted || m_isCrashed)
    return;

  if (std::chrono::duration<float, std::milli>(std::chrono::system_clock::now() - m_speedCheckpoint) < std::chrono::milliseconds(m_speed))
    return;

  if (m_currentCommand < m_commands.size()) {
    // Update time
    m_currentTime = std::chrono::system_clock::now();

    if (m_currentTime - m_startTime > m_requestTimeout) {
      // Send request
      send(m_commands[m_currentCommand], m_leaderId);
      m_startTime = std::chrono::system_clock::now();
    }

    // Receive response
    std::optional<MPI_Status> status = checkForMessage();

    if (!status.has_value()) return;

    ResponseToClient response;

    // Handle responses
    if (status->MPI_TAG == Message::Type::CMD_LOAD_RESPONSE) {
      CMD::LoadResponse response(recv(*status));

      if (response.getSuccess()) m_filesUID.push_back(response.getFileUID());
    } else if (status->MPI_TAG == Message::Type::CMD_LIST_RESPONSE) {
      CMD::ListResponse response(recv(*status));
    } else if (status->MPI_TAG == Message::Type::CMD_DELETE_RESPONSE) {
      CMD::DeleteResponse response(recv(*status));
    } else if (status->MPI_TAG == Message::Type::CMD_APPEND_RESPONSE) {
      CMD::AppendResponse response(recv(*status));
    }

    if (response.getSuccess()) {
      m_currentCommand++;
    } else {
      m_leaderId = response.getLeaderId();
    }
  }
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

    return CMD::List(m_id);
  } else if (cmd == "DELETE") {
    if (tokens.size() < 2)
      throw std::invalid_argument(
          "Missing file index. Usage: DELETE <file_index>\n");
    else if (tokens.size() > 2)
      throw std::invalid_argument(
          "Too many arguments. Usage: DELETE <file_index>\n");

    int file_index = std::stoi(tokens[1]);
    return CMD::Delete(m_filesUID[file_index], m_id);
  } else if (cmd == "APPEND") {
    if (tokens.size() < 3)
      throw std::invalid_argument(
          "Missing file index and/or data. Usage: APPEND <file_index> "
          "<data>\n");

    int file_index = std::stoi(tokens[1]);
    std::string data = tokens[2];
    return CMD::Append(m_filesUID[file_index], data, m_id);
  } else {
    throw std::invalid_argument("Unknown command");
  }
}
