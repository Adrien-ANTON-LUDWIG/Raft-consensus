#include "info.hh"

#include <iostream>

namespace MessageNS::REPL {
Info::Info(int originId)
    : Message(Type::REPL_INFO, originId){

      };

Info::Info(const json& data) : Message(data) {}

InfoResponse::InfoResponse(SpeedType speed, bool isCrashed, bool isClient,
                           bool isStarted, int originId)
    : Message(Type::REPL_INFO_RESPONSE, originId) {
  m_speed = speed;
  m_isCrashed = isCrashed, m_isClient = isClient;
  m_isStarted = isStarted;
}

InfoResponse::InfoResponse(const json& data) : Message(data) {
  m_speed = data["speed"];
  m_isCrashed = data["isCrashed"];
  m_isClient = data["isClient"];
  m_isStarted = data["isStarted"];
}

json InfoResponse::toJSON() const {
  json data = Message::toJSON();

  data["speed"] = m_speed;
  data["isCrashed"] = m_isCrashed;
  data["isClient"] = m_isClient;
  data["isStarted"] = m_isStarted;

  return data;
}

void InfoResponse::print() {
  auto& os = std::cout;
  if (m_isClient)
    os << "Client";
  else
    os << "Server";

  os << " (rank " << m_originId << ") infos:\n- Speed: ";
  if (m_speed == SpeedType::HIGH)
    os << "HIGH";
  else if (m_speed == SpeedType::MEDIUM)
    os << "MEDIUM";
  else
    os << "LOW";

  os << "\n- Is crashed: " << (m_isCrashed ? "YES" : "NO") << "\n";

  if (m_isClient)
    os << "- Is started: " << (m_isStarted ? "YES" : "NO") << "\n";
}
}  // namespace MessageNS::REPL
