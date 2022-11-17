#include "repl.hh"

#include <iostream>
#include <sstream>
#include <vector>

namespace REPL {
static bool isRunning = false;

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
  } else {
    std::cerr << "Unkown command.\n";
  }
}

void start() {
  std::cout << "REPL CLI enabled\n";
  isRunning = true;
  while (isRunning) {
    std::string line;
    if (std::getline(std::cin, line)) {
      parseCommand(line);
    }
  }
}

void stop() {
  isRunning = false;
  std::cout << "REPL CLI disabled.\n";
}

void headlessExec(const std::string &command) { parseCommand(command); }
}  // namespace REPL