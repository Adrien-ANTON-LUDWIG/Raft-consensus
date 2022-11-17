#pragma once

#include "messages/REPL/crash.hh"
#include "messages/REPL/speed.hh"

namespace REPL {

class Process {
 protected:
  MessageNS::REPL::SpeedType m_speed = MessageNS::REPL::SpeedType::HIGH;
  bool m_isCrashed = false;
  int m_replRank;

 public:
  Process(int replRank);
  void handleREPLSpeed(const json& json);
  void handleREPLCrash(const json& json);
  virtual void handleREPLInfo(const json& json) = 0;
};
}  // namespace REPL
