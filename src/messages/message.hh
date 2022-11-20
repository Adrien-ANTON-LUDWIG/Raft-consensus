#pragma once

#include <string>

#include "json.hpp"
#include "uuid.h"

using json = nlohmann::json;

namespace MessageNS {
/// @brief Base class of all messages.
class Message {
 public:
  enum Type {
    REDIRECT,

    // RPCs
    RPC_REQUEST_VOTE,
    RPC_VOTE,
    RPC_APPEND_ENTRIES,
    RPC_APPEND_ENTRIES_RESPONSE,

    // REPL
    REPL_SPEED,
    REPL_CRASH,
    REPL_START,
    REPL_INFO,
    REPL_INFO_RESPONSE,
    REPL_STOP,

    // CMDs
    CMD_LOAD,
    CMD_LIST,
    CMD_DELETE,
    CMD_APPEND,
    CMD_LOAD_RESPONSE,
    CMD_LIST_RESPONSE,
    CMD_DELETE_RESPONSE,
    CMD_APPEND_RESPONSE,
  };

  static bool isCMD(int type);

 private:
  uuids::uuid m_uuid;
  Type m_type;

 protected:
  int m_originId;

 public:
  Message() = default;
  Message(Type type, int originId);
  Message(const json& data);
  virtual ~Message() = default;

  std::string UUIDToStr() const;
  Type getType() const;
  static Type getType(const json& data);
  int getOriginId() const;

  virtual json toJSON() const;
};
}  // namespace MessageNS