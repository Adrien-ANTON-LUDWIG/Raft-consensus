#include "responseToClient.hh"

namespace MessageNS {
ResponseToClient::ResponseToClient(int leaderId, bool success, Type type,
                                   int originId)
    : Message(type, originId) {
  m_leaderId = leaderId;
  m_success = success;
}

ResponseToClient::ResponseToClient(const json& data) : Message(data) {
  m_leaderId = data["leaderId"];
  m_success = data["success"];
}

int ResponseToClient::getLeaderId() const { return m_leaderId; }

bool ResponseToClient::getSuccess() const { return m_success; }

json ResponseToClient::toJSON() const {
  json data = Message::toJSON();

  data["leaderId"] = m_leaderId;
  data["success"] = m_success;

  return data;
}
}  // namespace MessageNS
