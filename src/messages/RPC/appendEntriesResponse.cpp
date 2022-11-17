#include "appendEntriesResponse.hh"

namespace MessageNS::RPC {
AppendEntriesResponse::AppendEntriesResponse(int term, bool success,
                                             int originId)
    : Message(Type::RPC_APPEND_ENTRIES_RESPONSE, originId),
      m_success(success) {}

AppendEntriesResponse::AppendEntriesResponse(const json& data) : Message(data) {
  m_term = data["term"];
  m_success = data["success"];
  m_matchIndex = data["matchIndex"];
}

bool AppendEntriesResponse::isSuccess() const { return m_success; }
int AppendEntriesResponse::getTerm() const { return m_term; }
int AppendEntriesResponse::getMatchIndex() const { return m_matchIndex; }

void AppendEntriesResponse::setSuccess(bool success) { m_success = success; }
void AppendEntriesResponse::setMatchIndex(int matchIndex) {
  m_matchIndex = matchIndex;
}

json AppendEntriesResponse::toJSON() const {
  json data = Message::toJSON();

  data["term"] = m_term;
  data["success"] = m_success;
  data["matchIndex"] = m_matchIndex;

  return data;
}
}  // namespace MessageNS::RPC