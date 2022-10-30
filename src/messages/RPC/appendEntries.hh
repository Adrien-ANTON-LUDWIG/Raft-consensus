#pragma once

#include <string>
#include <vector>

#include "../message.hh"

namespace Message::RPC
{
    class AppendEntries : Message {
    private:
        int m_term;
        int m_leaderId;
        int m_prevLogIndex;
        int m_prevLogTerm;
        std::vector<std::string> m_entries;
        int m_leaderCommit;
    
    public:
        AppendEntries(int term, int leaderId, int prevLogIndex, int prevLogTerm,
                      const std::vector<std::string>& entries, int leaderCommit,
                      int originId);

        AppendEntries(int term, int leaderId, int prevLogIndex, int prevLogTerm,
                      int leaderCommit, int originId);

        AppendEntries(const json& data);

        int getTerm() const;
        int getLeader() const;
        int getPreviousLogIdx() const;
        int getPreviousLogTerm() const;
        const std::vector<std::string>& getEntries() const;
        int getLeaderCommit() const;

        AppendEntries& addEntry(const std::string& entry);

        virtual json toJSON() const override;
    };
} // namespace Message::RPC
