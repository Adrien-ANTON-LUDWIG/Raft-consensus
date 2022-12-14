#pragma once

#include "message.hh"

namespace MessageNS {
/// @brief Base class to every messages used to respond to a client.
class ResponseToClient : public Message {
 protected:
  int m_leaderId;
  bool m_success;

  ResponseToClient(int leaderId, bool success, Type type, int originId);
  ResponseToClient(const json& data);

 public:
  ResponseToClient() = default;

  int getLeaderId() const;
  bool getSuccess() const;

  virtual json toJSON() const override;
};
}  // namespace MessageNS
