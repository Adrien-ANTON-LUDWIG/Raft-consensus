#pragma once

#include "../message.hh"

namespace MessageNS::REPL {
enum SpeedType { LOW = 350, MEDIUM = 200, HIGH = 0 };

class Speed : public Message {
 private:
  SpeedType m_speed;

 public:
  Speed(SpeedType type, int originId);
  Speed(const json& data);

  json toJSON() const override;
};
}  // namespace MessageNS::REPL