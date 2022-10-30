#pragma once

#include "message.hh"

namespace Message
{
    class RaftVote : Message {
    private:
        int m_serverId;
    public:
        RaftVote(int serverId);
        RaftVote(const json& jsonStr);

        virtual json toJSON() const override;
    };
    
} // namespace Message
