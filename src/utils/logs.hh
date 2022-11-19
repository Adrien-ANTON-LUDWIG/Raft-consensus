#pragma once

#include "json.hpp"

using json = nlohmann::json;

class Logs {
 public:
  class Log {
   private:
    int m_term;
    json m_command;

   public:
    // Default constructor, needed for json deserialization
    Log(){};
    Log(int term, const json& command);

    int getTerm() const;
    json getCommand() const;

    // Macro that defines the to_json and from_json functions, provided by the
    // nlohmann::json library.
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Log, m_term, m_command)
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

  void addLog(int term, const json& command);
  void addLogs(const std::vector<Log>& logs);
  void commitLog(int index);
  void apply();

  Log getLog(int index);
  int getTerm(int index) const;

  // Get all logs from startIndex to the end.
  std::vector<Log> getLastLogs(int startIndex);
  void deleteLastLogs(int startIndex);

  int getCommitIndex();
  int getLastApplied();
  int getLastIndex();

  void updateCommitIndex(int leaderCommitIndex);

  bool contains(int index) const;

  void writeLogs(int serverID);
};
