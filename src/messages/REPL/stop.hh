#pragma once

#include "../message.hh"

namespace MessageNS::REPL {
/// @brief Message sent when closing the application.
class Stop : public Message {
 public:
  Stop(int originId);
  Stop(const json& data);
};
}  // namespace MessageNS::REPL
