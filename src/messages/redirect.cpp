#include "redirect.hh"

namespace MessageNS {
Redirect::Redirect(int leaderId, bool success, int originId) : ResponseToClient(leaderId, success, Type::REDIRECT, originId) {

}

Redirect::Redirect(const json& data) : ResponseToClient(data) {

}

}  // namespace MessageNS
