#pragma once

#include "messages/REPL/crash.hh"
#include "messages/REPL/speed.hh"

namespace REPL {

class Process {
 protected:
  MessageNS::REPL::SpeedType m_speed = MessageNS::REPL::SpeedType::HIGH;
  bool m_isRunning = false;
  bool m_isCrashed = false;
  int m_replRank;
  std::chrono::_V2::system_clock::time_point m_speedCheckpoint;

 public:
  Process(int replRank);
  /// @brief Manage REPL messages
  virtual void checkREPL() = 0;
  void handleREPLSpeed(const json& json);
  void handleREPLCrash(const json& json);
  void handleREPLRecovery(const json& json);
  virtual void handleREPLInfo(const json& json) = 0;
};
}  // namespace REPL
