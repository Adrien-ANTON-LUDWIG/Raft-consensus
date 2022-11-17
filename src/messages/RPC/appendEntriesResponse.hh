#pragma once

#include "../message.hh"

namespace MessageNS::RPC {
class AppendEntriesResponse : public Message {
 private:
  int m_term;
  bool m_success;
  int m_matchIndex = -1;

 public:
  AppendEntriesResponse(int term, bool success, int originId);
  AppendEntriesResponse(const json& data);

  bool isSuccess() const;
  int getTerm() const;
  int getMatchIndex() const;

  void setSuccess(bool success);
  void setMatchIndex(int matchIndex);

  virtual json toJSON() const override;
};
}  // namespace MessageNS::RPC