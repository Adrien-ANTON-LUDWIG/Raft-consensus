#include "list.hh"

namespace MessageNS::CMD {

List::List(int originId) : Message(Type::CMD_LIST, originId) {}

List::List(const json& data) : Message(data) {}

}  // namespace MessageNS::CMD