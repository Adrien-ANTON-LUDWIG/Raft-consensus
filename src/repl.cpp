#include "repl.hh"

#include <iostream>
#include <sstream>
#include <vector>

namespace REPL {
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
  std::cout << "exit                             Exit REPL CLI\n";
  std::cout << "help                             Display help\n";
}

static void parseCommand(const std::string &line) {
  std::istringstream iss(line);
  std::vector<std::string> tokens;
  std::string t;
  while (iss >> t) tokens.push_back(t);

  auto &cmd = tokens[0];
  if (cmd == "speed") {
    if (tokens.size() != 2) {
      std::cerr << "Missing speed type. Usage: speed low|medium|high\n";
      return;
    }

    auto &type = tokens[1];
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
  } else if (cmd == "crash") {
  } else if (cmd == "exit") {
    stop();
  } else if (cmd == "help") {
    printHelp();
  } else {
    std::cerr << "Unkown command.\n";
  }
}

void start(int clientCount, int serverCount) {
  std::cout << "REPL CLI enabled\n";
  g_clientCount = clientCount;
  g_serverCount = serverCount;
  printRanks();
  g_isRunning = true;
  while (g_isRunning) {
    std::string line;
    if (std::getline(std::cin, line)) {
      parseCommand(line);
    }
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