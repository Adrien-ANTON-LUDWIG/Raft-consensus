#include "repl_process.hh"

namespace REPL {
Process::Process(int replRank) {
  m_replRank = replRank;
}

void Process::handleREPLSpeed(const json& json) {
  MessageNS::REPL::Speed speed(json);

  m_speed = speed.getSpeed();
}

void Process::handleREPLCrash(__attribute__ ((unused)) const json& json) {
  //MessageNS::REPL::Crash crash(json);

  m_isCrashed = true;
}
}  // namespace REPL
