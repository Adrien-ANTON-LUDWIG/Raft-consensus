#pragma once

#include "../responseToClient.hh"

namespace MessageNS::CMD {
  class ListResponse : public ResponseToClient {
   private:
    std::vector<uuids::uuid> m_filesUIDs;

   public:
    ListResponse(const std::vector<uuids::uuid>& filesUIDs, int leaderId,
                 bool success, int originId);

    ListResponse(const json& data);

    std::vector<uuids::uuid> getFileUIDs() const;

    virtual json toJSON() const override;
  };
}  // namespace MessageNS::CMD