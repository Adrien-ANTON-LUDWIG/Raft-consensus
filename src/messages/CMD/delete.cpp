#include "delete.hh"

namespace MessageNS::CMD {

Delete::Delete(const uuids::uuid& fileUID, int originId)
    : Message(Type::CMD_DELETE, originId), m_fileUID(fileUID) {}

Delete::Delete(const json& data) : Message(data) {
  std::string fileUidStr = data["fileUID"];
  m_fileUID = uuids::uuid::from_string(fileUidStr).value();
}

uuids::uuid Delete::getFileUID() const { return m_fileUID; }

json Delete::toJSON() const {
  json data = Message::toJSON();

  data["fileUID"] = uuids::to_string(m_fileUID);

  return data;
}
}  // namespace MessageNS::CMD