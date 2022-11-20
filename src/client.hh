#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "worlds_info.hh"

#include "messages/message.hh"
#include "repl_process.hh"
#include "uuid.h"

class Client : ::REPL::Process {
 private:
  /**
   * State
   */
  Universe m_universe;
  int m_nbServer;
  int m_leaderId = 0;

  /**
   * Commands management
   */
  std::vector<MessageNS::Message*> m_commands;
  long unsigned int m_currentCommand = 0;
  std::vector<uuids::uuid> m_filesUID;

  /**
   * Time management
   */
  std::chrono::milliseconds m_requestTimeout;
  std::chrono::_V2::system_clock::time_point m_startTime;
  std::chrono::_V2::system_clock::time_point m_currentTime;

  /**
   * REPL related attributes
   */
  bool m_isCrashed;
  bool m_isStarted;

 public:
  Client(Universe universe, int nbServer, int replRank);
  ~Client();

  virtual void checkREPL() override;

  /// @brief Main loop of the client
  void run();

  /// @brief Load a commands file
  /// @param path 
  void loadCommands(const std::string& path);
  /// @brief Parse a command instruction
  /// @param command 
  /// @return The message to send
  MessageNS::Message* parseCommand(const std::string& command);

  // REPL HANDLERS
  virtual void handleREPLInfo(const json& json) override;
  void handleREPLStart(const json& json);
};