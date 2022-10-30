#pragma once

#include <string>
#include "json.hpp"
#include "uuid.h"

using json = nlohmann::json;

namespace Message
{
    class Message {
    public:
        enum Type {
            RAFT_VOTE,
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

    public:
        Message(const Type type);
        Message(const json& data);

        Type getType() const;
        std::string UUIDToStr() const;

        virtual json toJSON() const;
    };
}  // namespace Message