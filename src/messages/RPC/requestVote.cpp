#pragma once

#include "requestVote.hh"

namespace Message::RPC
{
    RequestVote::RequestVote(int term, bool candidateId, int originId) : Message(Type::RPC_REQUEST_VOTE, originId) {
        m_term = term;
        m_candidateId = candidateId;
    }

    RequestVote::RequestVote(const json& data) : Message(data) {
        m_term = data["term"];
        m_candidateId = data["candidate"];
    }

    int RequestVote::getTerm() const {
        return m_term;
    }

    int RequestVote::getCandidate() const {
        return m_candidateId;
    }

    json RequestVote::toJSON() const {
        json data = Message::toJSON();
        data["term"] = m_term;
        data["candidate"] = m_candidateId;
    }
} // namespace Message::RPC
