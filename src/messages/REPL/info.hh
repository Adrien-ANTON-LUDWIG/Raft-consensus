#pragma once

#include "../message.hh"
#include "speed.hh"

namespace MessageNS::REPL {
class Info : public Message {
 public:
  Info(int originId);
  Info(const json& data);
};

class InfoResponse : public Message {
 private:
  SpeedType m_speed;
  bool m_isCrashed;
  bool m_isClient;
  bool m_isStarted;
  bool m_isLeader;

 public:
  InfoResponse(SpeedType speed, bool isCrashed, bool isClient, bool isStarted, bool isLeader,
               int originId);
  InfoResponse(const json& data);

  json toJSON() const override;

  void print();
};
}  // namespace MessageNS::REPL
