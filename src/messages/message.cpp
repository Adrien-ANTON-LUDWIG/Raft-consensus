#include "message.hh"

namespace MessageNS
{
    Message::Message(Message::Type type, int originId) {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{generator};

        m_uuid = gen();
        m_type = type;
        m_originId = originId;
    }

    Message::Message(const json& data) {
        std::string uuidStr = data["uuid"];
        m_uuid = uuids::uuid::from_string(uuidStr).value();
        m_type = data["type"];
        m_originId = data["origin"];
    }

    std::string Message::UUIDToStr() const {
        return uuids::to_string(m_uuid);
    }

    Message::Type Message::getType() const {
        return m_type;
    }

    Message::Type Message::getType(const json& data) {
        return Type(data["type"]);
    }

    int Message::getOriginId() const {
        return m_originId;
    }

    json Message::toJSON() const {
        json data;
        data["uuid"] = UUIDToStr();
        data["type"] = m_type;
        data["origin"] = m_originId;

        return data;
    }

    bool Message::isCMD(int type) {
        return type == CMD_LOAD || type == CMD_LIST || type == CMD_DELETE || type == CMD_APPEND;
    }
} // namespace MessageNS