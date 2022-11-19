#include "stop.hh"

namespace MessageNS::REPL {
Stop::Stop(int originId) : Message(Type::REPL_STOP, originId) {}

Stop::Stop(const json& data) : Message(data) {}
}  // namespace MessageNS::REPL
