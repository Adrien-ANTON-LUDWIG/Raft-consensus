#pragma once

#include "../message.hh"

namespace Message::RPC
{
    class RequestVote : Message {
    private:
        int m_term;
        int m_candidateId;
        // TODO log entry

    public:
        RequestVote(int term, bool candidateId);
        RequestVote(const json& data);

        int getTerm() const;
        int getCandidate() const;

        virtual json toJSON() const override;
    };
} // namespace Message
