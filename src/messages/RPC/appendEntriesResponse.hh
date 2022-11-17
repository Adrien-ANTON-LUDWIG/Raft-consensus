#pragma once

#include "../message.hh"

namespace MessageNS::RPC {
class AppendEntriesResponse : public Message {
 private:
  int m_term;
  bool m_success;

 public:
  AppendEntriesResponse(int term, bool success, int originId);

  bool isSuccess() const;
  int getTerm() const;

  virtual json toJSON() const override;
};
}  // namespace MessageNS::RPC