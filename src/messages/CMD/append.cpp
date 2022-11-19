#include "append.hh"

namespace MessageNS::CMD {
Append::Append(const uuids::uuid& fileUID, const std::string& content,
               int originId)
    : Message(Type::CMD_APPEND, originId),
      m_fileUID(fileUID),
      m_content(content) {}

Append::Append(const json& data) : Message(data) {
  std::string fileUidStr = data["fileUID"];
  m_fileUID = uuids::uuid::from_string(fileUidStr).value();
  m_content = data["content"];
}

uuids::uuid Append::getFileUID() const { return m_fileUID; }
std::string Append::getContent() const { return m_content; }

json Append::toJSON() const {
  json data = Message::toJSON();

  data["fileUID"] = uuids::to_string(m_fileUID);
  data["content"] = m_content;

  return data;
}
}  // namespace MessageNS::CMD