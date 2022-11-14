#pragma once

#include "../message.hh"

namespace Message::RPC
{
    class Vote : public Message {
    private:
        int m_term;
        bool m_granted;

    public:
        Vote(int term, bool granted, int originId);
        Vote(const json& data);

        bool isGranted() const;
        int getTerm() const;

        virtual json toJSON() const override;
    };
    
} // namespace Message::RPC
