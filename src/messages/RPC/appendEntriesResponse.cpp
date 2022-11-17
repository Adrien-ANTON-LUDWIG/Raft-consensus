#include "appendEntriesResponse.hh"

namespace MessageNS::RPC {
AppendEntriesResponse::AppendEntriesResponse(int term, bool success,
                                             int originId)
    : Message(Type::RPC_APPEND_ENTRIES_RESPONSE, originId),
      m_success(success) {}

bool AppendEntriesResponse::isSuccess() const { return m_success; }
int AppendEntriesResponse::getTerm() const { return m_term; }

json AppendEntriesResponse::toJSON() const {
  json data = Message::toJSON();

  data["term"] = m_term;
  data["success"] = m_success;

  return data;
}
}  // namespace MessageNS::RPC