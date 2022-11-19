#pragma once

#include <string>

#include "json.hpp"
#include "uuid.h"

using json = nlohmann::json;

namespace MessageNS {
class Message {
 public:
  enum Type {
    STATUS,
    RPC_REQUEST_VOTE,
    RPC_VOTE,
    RPC_APPEND_ENTRIES,
    RPC_APPEND_ENTRIES_RESPONSE,
    REPL_SPEED,
    REPL_CRASH,
    REPL_START,
    REPL_INFO,
    REPL_STOP,
    CMD_LOAD,
    CMD_LIST,
    CMD_DELETE,
    CMD_APPEND,
    RESP_CMD_LOAD,
    RESP_REPL_INFO,
  };

  static bool isCMD(int type);

 private:
  uuids::uuid m_uuid;
  Type m_type;

 protected:
  int m_originId;

 public:
  Message(Type type, int originId);
  Message(const json& data);

  std::string UUIDToStr() const;
  Type getType() const;
  static Type getType(const json& data);
  int getOriginId() const;

  virtual json toJSON() const;
};
}  // namespace MessageNS