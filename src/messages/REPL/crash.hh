#pragma once

#include "../message.hh"

namespace MessageNS::REPL {

class Crash : public Message {
 public:
  Crash(int originId);
  Crash(const json& data);

  json toJSON() const override;
};
}  // namespace MessageNS::REPL