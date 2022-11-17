#include "repl.hh"

#include <iostream>
#include <sstream>
#include <vector>

#include "messages/mpi_wrappers.hh"
#include "messages/REPL/info.hh"

namespace REPL {
static int g_rank = -1;
static bool g_isRunning = false;
static int g_clientCount = -1;
static int g_serverCount = -1;
static bool g_isHeadlessRun = false;

static void printRanks() {
  std::cout << "Client ranks: [0, " << g_clientCount - 1 << "] | Server ranks: [" << g_clientCount << ", " << g_clientCount + g_serverCount - 1 << "]\n";
}

static void printHelp() {
  std::cout << "----HELP----\n";
  printRanks();
  std::cout << "speed <rank> low|medium|high     Change speed of process of rank <rank>\n";
  std::cout << "start <rank>                     Start client of rank <rank>\n";
  std::cout << "crash <rank>                     Crash process of rank <rank>\n";
  std::cout << "info  <rank>                     Get REPL infos of the process of rank <rank>\n";
  std::cout << "exit                             Exit REPL CLI\n";
  std::cout << "help                             Display help\n";
}

static bool parseRank(std::string t, int &rank) {
    try
    {
      rank = std::stoi(t);
      if (rank < 0 || rank >= g_serverCount + g_clientCount)
        throw std::exception();
    } catch (std::exception& e) {
      std::cerr << "Rank is not a valid rank.\n";
      return false;
    }

    return true;
}

static bool getResponse(const MessageNS::Message& query, int rank, MessageNS::Message::Type type, json& response) {
    send(query, rank);

    response = waitForResponse(rank);
    if (auto responseType = MessageNS::Message::getType(response) != type)
    {
      std::cerr << "Implementation error. Bad response type " << responseType << ".\n";
      return false;
    }

    return true;
}

static void parseCommand(const std::string &line) {
  std::istringstream iss(line);
  std::vector<std::string> tokens;
  std::string t;
  while (iss >> t) tokens.push_back(t);

  int rank = -1;
  auto &cmd = tokens[0];
  if (cmd == "speed") {
    if (tokens.size() < 3) {
      std::cerr << "Missing speed type. Usage: speed low|medium|high\n";
      return;
    }

    if (!parseRank(tokens[1], rank))
      return;

    auto &type = tokens[2];
    if (type == "low") {
      // TODO
    } else if (type == "medium") {
      // TODO
    } else if (type == "high") {
      // TODO
    } else {
      std::cerr << "Invalid speed type. Usage: speed low|medium|high\n";
    }
  } else if (cmd == "start") {
    if (tokens.size() < 2) {
      std::cerr << "Missing rank. Usage: start <rank>\n";
      return;
    }

    if (!parseRank(tokens[1], rank))
      return;



  } else if (cmd == "crash") {
    if (tokens.size() < 2) {
      std::cerr << "Missing rank. Usage: crash <rank>\n";
      return;
    }

    if (!parseRank(tokens[1], rank))
      return;

    
  } else if (cmd == "info") {
    if (tokens.size() < 2) {
      std::cerr << "Missing rank. Usage: info <rank>\n";
      return;
    }

    if (!parseRank(tokens[1], rank))
      return;

    MessageNS::REPL::Info query(g_rank);
    json response;
    if (!getResponse(query, rank, MessageNS::Message::Type::RESP_REPL_INFO, response)) return;

    MessageNS::REPL::InfoResponse infos(response);
    infos.print();
  } else if (cmd == "exit") {
    stop();
  } else if (cmd == "help") {
    printHelp();
  } else {
    std::cerr << "Unknown command.\n";
  }
}

void init(int rank) {
  g_rank = rank;
}

void start(int clientCount, int serverCount) {
  std::cout << "REPL CLI enabled\n";
  g_clientCount = clientCount;
  g_serverCount = serverCount;
  printRanks();
  g_isRunning = true;
  std::cout << "> ";
  while (g_isRunning) {
    std::string line;
    if (std::getline(std::cin, line)) {
      if (line.size() != 0)
        parseCommand(line);
    }
    std::cout << "> ";
  }
}

void stop() {
  g_isRunning = false;
  std::cout << "REPL CLI disabled.\n";
}

void headlessExec(const std::string &command) {
  g_isHeadlessRun = true;
  parseCommand(command);
}
}  // namespace REPL