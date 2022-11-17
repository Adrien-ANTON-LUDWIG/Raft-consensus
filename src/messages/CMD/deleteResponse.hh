#pragma once

#include "../responseToClient.hh"

namespace MessageNS::CMD {
class DeleteResponse : public ResponseToClient {
 public:
  DeleteResponse(int leaderId, bool success, int originId);
  DeleteResponse(const json& data);
};
}  // namespace MessageNS::CMD