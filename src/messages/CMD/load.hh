#pragma once

#include "../message.hh"

namespace MessageNS::CMD {
class Load : public Message {
 private:
  std::string m_filename;

 public:
  Load(std::string filename, int originId);
  Load(const json& data);

  std::string getFilename() const;

  json toJSON() const override;
};
}  // namespace MessageNS::CMD