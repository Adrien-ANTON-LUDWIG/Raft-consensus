#pragma once

#include "json.hpp"

using json = nlohmann::json;

class Logs {
  class Log {
   private:
    int m_term;
    json m_command;

   public:
    Log(int term, const json &command);

    int getTerm();
    json getCommand();
  };

 private:
  // Each entry contains command for state machine, and term when entry was
  // received by leader.
  std::vector<Log> m_log;

  // Index of the highest log entry known to be committed.
  int m_commitIndex = 0;

  // Index of the highest log entry applied to the state machine.
  int m_lastApplied = 0;

 public:
  Logs();

  void addLog(int term, const json &command);
  void commitLog(int index);
  void apply();

  Log getLog(int index);
  int getCommitIndex();
  int getLastApplied();
};