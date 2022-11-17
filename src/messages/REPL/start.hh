#pragma once

#include "../message.hh"

namespace MessageNS::REPL {

class Start : public Message {
 public:
  Start(int originId);
  Start(const json& data);

  json toJSON() const override;
};
}  // namespace MessageNS::REPL