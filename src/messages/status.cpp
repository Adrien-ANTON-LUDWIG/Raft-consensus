#include "status.hh"

namespace MessageNS
{
    Status::Status(int term, bool success, int originId) : Message(Type::STATUS, originId) {
        m_term = term;
        m_success = success;
    }

    Status::Status(const json& data) : Message(data) {
        m_term = data["term"];
        m_success = data["success"];
    }

    int Status::getTerm() const {
        return m_term;
    }

    int Status::isSuccess() const {
        return m_success;
    }

    json Status::toJSON() const {
        json data = Message::toJSON();

        data["term"] = m_term;
        data["success"] = m_success;

        return data;
    }
} // namespace MessageNS
