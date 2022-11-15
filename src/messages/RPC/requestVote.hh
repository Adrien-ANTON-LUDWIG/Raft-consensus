#pragma once

#include "../message.hh"

namespace MessageNS::RPC
{
    class RequestVote : public Message {
    private:
        int m_term;
        int m_candidateId;
        // TODO log entry

    public:
        RequestVote(int term, int candidateId);
        RequestVote(const json& data);

        int getTerm() const;
        int getCandidate() const;

        virtual json toJSON() const override;
    };
} // namespace MessageNS
