#pragma once

#include "../message.hh"

namespace MessageNS::CMD {
class Append : public Message {
 private:
  uuids::uuid m_fileUID;
  std::string m_content;

 public:
  Append(const uuids::uuid& fileUID, const std::string& content, int originId);
  Append(const json& data);

  uuids::uuid getFileUID() const;
  std::string getContent() const;

  json toJSON() const override;
};
}  // namespace MessageNS::CMD