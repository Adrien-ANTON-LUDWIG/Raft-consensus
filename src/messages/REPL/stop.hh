#pragma once

#include "../message.hh"

namespace MessageNS::REPL {
class Stop : public Message {
 public:
  Stop(int originId);
  Stop(const json& data);
};
}  // namespace MessageNS::REPL
