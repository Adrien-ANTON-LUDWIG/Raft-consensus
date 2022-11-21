#pragma once

#include "../message.hh"

namespace MessageNS::REPL {

class Recovery : public Message {
 public:
  Recovery(int originId);
  Recovery(const json& data);
};
}  // namespace MessageNS::REPL