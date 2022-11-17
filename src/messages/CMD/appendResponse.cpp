#include "appendResponse.hh"

namespace MessageNS::CMD {

AppendResponse::AppendResponse(int leaderId, bool success, int originId)
    : ResponseToClient(leaderId, success, Type::CMD_APPEND_RESPONSE, originId) {
}

AppendResponse::AppendResponse(const json& data) : ResponseToClient(data) {}

}  // namespace MessageNS::CMD