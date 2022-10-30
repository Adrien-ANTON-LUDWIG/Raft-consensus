#include "appendEntries.hh"

namespace Message::RPC
{
    AppendEntries::AppendEntries(int term, int leaderId, int prevLogIndex,
                                 int prevLogTerm, const std::vector<std::string>& entries,
                                 int leaderCommit, int originId) : Message(Type::RPC_APPEND_ENTRIES, originId) {
        m_term = term;
        m_leaderId = leaderId;
        m_prevLogIndex = prevLogIndex;
        m_prevLogTerm = prevLogTerm;
        m_entries = entries;
        m_leaderCommit = leaderCommit;
    }

    AppendEntries::AppendEntries(int term, int leaderId, int prevLogIndex,
                                 int prevLogTerm, int leaderCommit,
                                 int originId) : Message(Type::RPC_APPEND_ENTRIES, originId) {
        m_term = term;
        m_leaderId = leaderId;
        m_prevLogIndex = prevLogIndex;
        m_prevLogTerm = prevLogTerm;
        m_entries = {};
        m_leaderCommit = leaderCommit;
    }

    AppendEntries::AppendEntries(const json& data) : Message(data) {
        m_term = data["term"];
        m_leaderId = data["leader"];
        m_prevLogIndex = data["prevLogIndex"];
        m_prevLogTerm = data["prevLogTerm"];
        m_entries = data["entries"];
        m_leaderCommit = data["leaderCommit"];
    }

    int AppendEntries::getTerm() const {
        return m_term;
    }

    int AppendEntries::getLeader() const {
        return m_leaderId;
    }

    int AppendEntries::getPreviousLogIdx() const {
        return m_prevLogIndex;
    }

    int AppendEntries::getPreviousLogTerm() const {
        return m_prevLogTerm;
    }

    const std::vector<std::string>& AppendEntries::getEntries() const {
        return m_entries;
    }

    int AppendEntries::getLeaderCommit() const {
        return m_leaderCommit;
    }

    AppendEntries& AppendEntries::addEntry(const std::string& entry) {
        m_entries.push_back(entry);

        return *this;
    }

    json AppendEntries::toJSON() const {
        json data = Message::toJSON();

        data["term"] = m_term;
        data["leader"] = m_leaderId;
        data["prevLogIndex"] = m_prevLogIndex;
        data["prevLogTerm"] = m_prevLogTerm;
        data["entries"] = m_entries;
        data["leaderCommit"] = m_leaderCommit;

        return data;
    }
} // namespace Message::RPC
