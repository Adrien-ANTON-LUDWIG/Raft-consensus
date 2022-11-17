#include "crash.hh"

namespace MessageNS::REPL {

  Crash::Crash(int originId) : Message(Type::REPL_CRASH, originId) {
  }

  Crash::Crash(const json& data) : Message(data) {
  }
}  // namespace MessageNS::REPL