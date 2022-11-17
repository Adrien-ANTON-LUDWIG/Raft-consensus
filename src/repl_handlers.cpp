#include "server.h"
#include "client.hh"

#include "messages/REPL/crash.hh"
#include "messages/REPL/info.hh"
#include "messages/REPL/speed.hh"
#include "messages/REPL/start.hh"

void Server::handleREPLInfo(const json& json) {
    MessageNS::REPL::Info info(json);

    MessageNS::REPL::InfoResponse response(m_speed, m_isCrashed, false, true, m_id);
    send(response, info.getOriginId());
}

void Client::handleREPLInfo(const json& json) {
    MessageNS::REPL::Info info(json);

    MessageNS::REPL::InfoResponse response(m_speed, m_isCrashed, true, m_isStarted, m_id);
    send(response, info.getOriginId());
}

void Client::handleREPLStart(__attribute__ ((unused)) const json& json) {
    // MessageNS::REPL::Start start(json);

    m_isStarted = true;
}