#include "redirect.hh"

namespace MessageNS {
Redirect::Redirect(int leaderId, int originId) : ResponseToClient(leaderId, false, Type::REDIRECT, originId) {

}

Redirect::Redirect(const json& data) : ResponseToClient(data) {

}

}  // namespace MessageNS
