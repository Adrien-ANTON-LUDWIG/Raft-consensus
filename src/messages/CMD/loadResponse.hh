#pragma once

#include "../responseToClient.hh"

namespace MessageNS::CMD {
class LoadResponse : public ResponseToClient {
 private:
  uuids::uuid m_fileUID;

 public:
  LoadResponse(const uuids::uuid& fileUID, int leaderId, bool success,
               int originId);
  LoadResponse(const json& data);

  uuids::uuid getFileUID() const;

  virtual json toJSON() const override;
};
}  // namespace MessageNS::CMD
