#include "raftVote.hh"

namespace Message
{
    RaftVote::RaftVote(int serverId) : Message(Type::RAFT_VOTE) {
        m_serverId = serverId;
    }

    RaftVote::RaftVote(const json& data) : Message(data) {
        m_serverId = data["serverId"];
    }

    json RaftVote::toJSON() const {
        json data = Message::toJSON();
        data["serverId"] = m_serverId;

        return data;
    }
} // namespace Message
