#pragma once

#include "responseToClient.hh"

namespace MessageNS {
class Redirect : public ResponseToClient {
  public:
    Redirect(int leaderId, int originId);
    Redirect(const json& data);
};
}  // namespace MessageNS
