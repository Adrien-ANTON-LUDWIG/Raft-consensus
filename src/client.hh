#pragma once

#include <string>
#include <vector>

#include "repl_process.hh"

class Client : REPL::Process {
private:
  int m_id;
  int m_nbServer;
  int m_leaderId = 0;

  std::vector<std::string> m_commands;

  // REPL
  bool m_isCrashed;
  bool m_isStarted;
  

public:
  Client(int id, int nbServer);

  void update();

  void loadCommands(const std::string& path);

  // REPL HANDLERS
  virtual void handleREPLInfo(const json& json) override;
  void handleREPLStart(const json& json);
};