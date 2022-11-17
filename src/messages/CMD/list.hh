#pragma once

#include "../message.hh"

namespace MessageNS::CMD {
class List : public Message {
 public:
  List(int originId);
  List(const json& data);
};
}  // namespace MessageNS::CMD
