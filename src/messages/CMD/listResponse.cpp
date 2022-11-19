#include "listResponse.hh"

namespace MessageNS::CMD {
ListResponse::ListResponse(const std::vector<uuids::uuid>& filesUIDs,
                           int leaderId, bool success, int originId)
    : ResponseToClient(leaderId, success, Type::CMD_LIST_RESPONSE, originId),
      m_filesUIDs(filesUIDs) {}

ListResponse::ListResponse(const json& data) : ResponseToClient(data) {
  for (std::string fileUidStr : data["filesUIDs"]) {
    m_filesUIDs.push_back(uuids::uuid::from_string(fileUidStr).value());
  }
}

std::vector<uuids::uuid> ListResponse::getFileUIDs() const {
  return m_filesUIDs;
}

json ListResponse::toJSON() const {
  json data = ResponseToClient::toJSON();

  std::vector<std::string> filesUIDsStr;
  for (uuids::uuid fileUID : m_filesUIDs)
    filesUIDsStr.push_back(uuids::to_string(fileUID));
  data["filesUIDs"] = filesUIDsStr;

  return data;
}
}  // namespace MessageNS::CMD