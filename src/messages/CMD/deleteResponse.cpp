#include "deleteResponse.hh"

namespace MessageNS::CMD {

DeleteResponse::DeleteResponse(int leaderId, bool success, int originId)
    : ResponseToClient(leaderId, success, Type::CMD_DELETE_RESPONSE, originId) {
}

DeleteResponse::DeleteResponse(const json& data) : ResponseToClient(data) {}

}  // namespace MessageNS::CMD