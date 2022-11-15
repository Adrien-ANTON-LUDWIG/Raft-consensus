#include "vote.hh"

namespace MessageNS::RPC {
Vote::Vote(int term, bool granted, int originId)
    : Message(Type::RPC_VOTE, originId) {
  m_term = term;
  m_granted = granted;
}

Vote::Vote(const json& data) : Message(data) {
  m_term = data["term"];
  m_granted = data["granted"];
}

bool Vote::isGranted() const { return m_granted; }

int Vote::getTerm() const { return m_term; }

json Vote::toJSON() const {
  json data = Message::toJSON();
  data["term"] = m_term;
  data["granted"] = m_granted;

  return data;
}
}  // namespace MessageNS::RPC
