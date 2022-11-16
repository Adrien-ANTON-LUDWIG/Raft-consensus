#pragma once

#include <string>
#include <vector>

#include "../message.hh"
#include "../../utils/logs.hh"

namespace MessageNS::RPC
{
    class AppendEntries : public Message {
    private:
        int m_term;
        int m_leaderId;
        int m_prevLogIndex;
        int m_prevLogTerm;
        std::vector<Logs::Log> m_entries;
        int m_leaderCommit;
    
    public:
        AppendEntries(int term, int leaderId, int prevLogIndex, int prevLogTerm,
                      const std::vector<Logs::Log>& entries, int leaderCommit);

        AppendEntries(int term, int leaderId, int prevLogIndex, int prevLogTerm,
                      int leaderCommit);

        AppendEntries(const json& data);

        int getTerm() const;
        int getLeader() const;
        int getPreviousLogIdx() const;
        int getPreviousLogTerm() const;
        const std::vector<Logs::Log>& getEntries() const;
        int getLeaderCommit() const;

        bool isHeartbeat() const;

        AppendEntries& addEntry(const Logs::Log& entry);

        virtual json toJSON() const override;
    };
} // namespace MessageNS::RPC
