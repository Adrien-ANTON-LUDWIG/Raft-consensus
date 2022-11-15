#pragma once

class Client {
private:
  int m_id;
  int m_nbServer;
  int m_leaderId = 0;

public:
  Client(int id, int nbServer);

  void update();
};