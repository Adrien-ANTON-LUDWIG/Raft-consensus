#include "load.hh"

namespace MessageNS::CMD {
Load::Load(std::string filename, int originId)
    : Message(Type::CMD_LOAD, originId) {
  m_filename = filename;
}

Load::Load(const json& data) : Message(data) { m_filename = data["filename"]; }

std::string Load::getFilename() const { return m_filename; }

json Load::toJSON() const {
  json data = Message::toJSON();
  data["filename"] = m_filename;

  return data;
}
}  // namespace MessageNS::CMD