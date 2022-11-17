#include "speed.hh"

namespace MessageNS::REPL
{
  Speed::Speed(SpeedType type, int originId) : Message(Type::REPL_SPEED, originId) {
    m_speed = type;
  }

  Speed::Speed(const json& data) : Message(data) {
    m_speed = data["speed"];
  }

  json Speed::toJSON() const {
    json data = Message::toJSON();

    data["speed"] = m_speed;

    return data;
  }
} // namespace MessageNS::REPL
