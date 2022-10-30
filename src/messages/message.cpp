#include "message.hh"

namespace Message
{
    Message::Message(const Message::Type type) {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{generator};

        m_uuid = gen();
        m_type = type;
    }

    Message::Message(const json& data) {
        std::string uuidStr = data["uuid"];
        m_uuid = uuids::uuid::from_string(uuidStr).value();
        m_type = data["type"];
    }

    Message::Type Message::getType() const {
        return m_type;
    }

    std::string Message::UUIDToStr() const {
        return uuids::to_string(m_uuid);
    }

    json Message::toJSON() const {
        json data;
        data["uuid"] = UUIDToStr();
        data["type"] = m_type;

        return data;
    }
} // namespace Message