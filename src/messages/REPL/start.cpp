#include "start.hh"

namespace MessageNS::REPL {

  Start::Start(int originId) : Message(Type::REPL_START, originId) {
  }

  Start::Start(const json& data) : Message(data) {
  }
}  // namespace MessageNS::REPL