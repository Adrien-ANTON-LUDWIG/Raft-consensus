#include "recovery.hh"

namespace MessageNS::REPL {
  Recovery::Recovery(int originId) : Message(Type::REPL_RECOVERY, originId) {
  }

  Recovery::Recovery(const json& data) : Message(data) {
  }
}  // namespace MessageNS::REPL