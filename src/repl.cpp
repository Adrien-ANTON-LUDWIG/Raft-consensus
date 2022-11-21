#include "repl.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>

#include "messages/REPL/crash.hh"
#include "messages/REPL/info.hh"
#include "messages/REPL/recovery.hh"
#include "messages/REPL/start.hh"
#include "messages/REPL/stop.hh"
#include "messages/mpi_wrappers.hh"

namespace REPL
{
  static Universe g_universe;
  static bool g_isRunning = false;
  static int g_clientCount = -1;
  static int g_serverCount = -1;
  static std::string g_commandsFile;

  static void printRanks()
  {
    std::cout << "Server ranks: [0, " << g_serverCount - 1
              << "] | Client ranks: [" << g_serverCount << ", "
              << g_clientCount + g_serverCount - 1 << "]" << std::endl;
  }

  static void printHelp()
  {
    std::cout << "----HELP----" << std::endl;
    ;
    printRanks();
    std::cout << "speed <rank> low|medium|high     Change speed of process of "
                 "rank <rank>\n";
    std::cout << "start <rank>                     Start client of rank <rank>\n";
    std::cout
        << "crash <rank>                     Crash process of rank <rank>\n";
    std::cout << "info  <rank>                     Get REPL infos of the process "
                 "of rank <rank>\n";
    std::cout << "exit                             Exit REPL CLI\n";
    std::cout << "help                             Display help" << std::endl;
  }

  static bool parseRank(std::string t, int &rank)
  {
    try
    {
      rank = std::stoi(t);
      if (rank < 0 || rank >= g_serverCount + g_clientCount)
        throw std::exception();
    }
    catch (std::exception &e)
    {
      std::cerr << "Rank is not a valid rank." << std::endl;
      return false;
    }

    return true;
  }

  static bool getResponse(const MessageNS::Message &query, int rank,
                          MessageNS::Message::Type type, json &response)
  {
    send(query, rank, g_universe.replWorld.com);

    while (waitForResponse(rank, g_universe.replWorld.com, response))
      ;
    if (auto responseType = MessageNS::Message::getType(response) != type)
    {
      std::cerr << "Implementation error. Bad response type " << responseType
                << "." << std::endl;
      return false;
    }

    return true;
  }

  static void parseCommand(const std::string &line)
  {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string t;
    while (iss >> t)
      tokens.push_back(t);

    int rank = -1;
    auto &cmd = tokens[0];
    if (cmd == "speed")
    {
      if (tokens.size() < 3)
      {
        std::cerr << "Ill formed command. Usage: speed <rank> low|medium|high"
                  << std::endl;
        return;
      }

      if (!parseRank(tokens[1], rank))
        return;

      auto &type = tokens[2];
      if (type == "low")
      {
        MessageNS::REPL::Speed message(MessageNS::REPL::SpeedType::LOW, g_universe.replWorld.rank);
        send(message, rank, g_universe.replWorld.com);
      }
      else if (type == "medium")
      {
        MessageNS::REPL::Speed message(MessageNS::REPL::SpeedType::MEDIUM,
                                       g_universe.replWorld.rank);
        send(message, rank, g_universe.replWorld.com);
      }
      else if (type == "high")
      {
        MessageNS::REPL::Speed message(MessageNS::REPL::SpeedType::HIGH, g_universe.replWorld.rank);
        send(message, rank, g_universe.replWorld.com);
      }
      else
      {
        std::cerr << "Invalid speed type. Usage: speed <rank> low|medium|high"
                  << std::endl;
      }
    }
    else if (cmd == "start")
    {
      if (tokens.size() < 2)
      {
        std::cerr << "Missing rank. Usage: start <rank>" << std::endl;
        return;
      }

      if (!parseRank(tokens[1], rank))
        return;

      if (rank < g_serverCount || rank >= g_serverCount + g_clientCount)
      {
        std::cerr << "Bad rank. Should be in range [" << g_serverCount << ","
                  << g_universe.replWorld.rank - 1 << "]." << std::endl;
        return;
      }

      MessageNS::REPL::Start message(g_universe.replWorld.rank);
      send(message, rank, g_universe.replWorld.com);
    }
    else if (cmd == "crash")
    {
      if (tokens.size() < 2)
      {
        std::cerr << "Missing rank. Usage: crash <rank>" << std::endl;
        return;
      }

      if (!parseRank(tokens[1], rank))
        return;

      MessageNS::REPL::Crash message(g_universe.replWorld.rank);
      send(message, rank, g_universe.replWorld.com);
    } else if (cmd == "recovery") {
    if (tokens.size() < 2) {
      std::cerr << "Missing rank. Usage: recovery <rank>" << std::endl;
      return;
    }

    if (!parseRank(tokens[1], rank)) return;

    if (rank < 0 || rank >= g_serverCount) {
      std::cerr << "Bad rank. Should be in range [" << g_serverCount << ","
                << g_universe.replWorld.rank - 1 << "]." << std::endl;
      return;
    }

    MessageNS::REPL::Recovery message(g_universe.replWorld.rank);
    send(message, rank, g_universe.replWorld.com);

  } else if (cmd == "info")
    {
      if (tokens.size() < 2)
      {
        std::cerr << "Missing rank. Usage: info <rank>" << std::endl;
        return;
      }

      if (!parseRank(tokens[1], rank))
        return;

      MessageNS::REPL::Info query(g_universe.replWorld.rank);
      json response;
      if (!getResponse(query, rank, MessageNS::Message::Type::REPL_INFO_RESPONSE,
                       response))
        return;

      MessageNS::REPL::InfoResponse infos(response);
      infos.print();
    }
    else if (cmd == "exit")
    {
      stop();
    }
    else if (cmd == "help")
    {
      printHelp();
    }
    else
    {
      std::cerr << "Unknown command." << std::endl;
    }
  }

  void init(Universe universe, const std::string &commandsFile)
  {
    g_universe = universe;
    g_commandsFile = commandsFile;
  }

  void start(int clientCount, int serverCount)
  {
    if (g_commandsFile != "")
    {
      std::ifstream stream(g_commandsFile);
      if (stream.is_open())
      {
        std::string line;
        bool headlessMode = false;
        while (getline(stream, line))
        {
          if (line[0] == '$')
          {
            g_isRunning = true;
            headlessMode = true;
            std::cout << "REPL running in headless mode\n";
          }
          else if (headlessMode)
          {
            std::cout << "REPL cmd: " << line << std::endl;
            if (line[0] == '@')
              std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(line.substr(1))));
            else
              parseCommand(line);
          }
        }

        if (headlessMode)
        {
          if (!g_isRunning)
            std::cout << "REPL disabled as headless mode\n";
          return;
        }
      }
    }

    std::cout << "REPL CLI enabled\n";
    g_clientCount = clientCount;
    g_serverCount = serverCount;
    printRanks();
    g_isRunning = true;
    std::cout << "> ";
    while (g_isRunning)
    {
      std::string line;
      if (std::getline(std::cin, line))
      {
        if (line.size() != 0)
          parseCommand(line);

        if (g_isRunning)
          std::cout << "> ";
      }
    }
    std::cout << "REPL CLI disabled.\n";
  }

  void stop()
  {
    g_isRunning = false;
    MessageNS::REPL::Stop message(g_universe.replWorld.rank);
    for (int rank = 0; rank <= g_clientCount + g_serverCount; rank++)
    {
      send(message, rank, g_universe.replWorld.com);
    }
  }
} // namespace REPL