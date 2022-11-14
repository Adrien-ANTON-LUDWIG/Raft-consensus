#pragma once

#include <string>
#include "json.hpp"
#include "uuid.h"

using json = nlohmann::json;

namespace MessageNS
{
    class Message {
    public:
        enum Type {
            STATUS,
            RPC_REQUEST_VOTE,
            RPC_VOTE,
            RPC_APPEND_ENTRIES,
            REPL_SPEED,
            REPL_CRASH,
            REPL_START,
            CMD_LOAD,
            CMD_LIST,
            CMD_DELETE,
            CMD_APPEND
        };

    private:
        uuids::uuid m_uuid;
        Type m_type;
        int m_originId;

    public:
        Message(Type type, int originId);
        Message(const json& data);

        Type getType() const;
        static Type getType(const json& data);
        std::string UUIDToStr() const;

        virtual json toJSON() const;
    };
}  // namespace MessageNS