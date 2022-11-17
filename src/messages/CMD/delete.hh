#pragma once

#include "../message.hh"

namespace MessageNS::CMD {
class Delete : public Message {
 private:
  uuids::uuid m_fileUID;

 public:
  Delete(const uuids::uuid& fileUID, int originId);
  Delete(const json& data);

  uuids::uuid getFileUID() const;

  json toJSON() const override;
};
}  // namespace MessageNS::CMD