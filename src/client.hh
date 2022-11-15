#pragma once

#include <string>
#include <vector>

class Client {
private:
  int m_id;
  int m_nbServer;
  int m_leaderId = 0;

  std::vector<std::string> m_commands;

public:
  Client(int id, int nbServer);

  void update();

  void loadCommands(const std::string& path);
};