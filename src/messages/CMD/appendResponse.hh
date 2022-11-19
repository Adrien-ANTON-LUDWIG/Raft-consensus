#pragma once

#include "../responseToClient.hh"

namespace MessageNS::CMD {
class AppendResponse : public ResponseToClient {
 public:
  AppendResponse(int leaderId, bool success, int originId);
  AppendResponse(const json& data);
};
}  // namespace MessageNS::CMD