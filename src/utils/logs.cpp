#include "logs.hh"

Logs::Log::Log(int term, const json &command) : m_term(term), m_command(command) {}

int Logs::Log::getTerm() { return m_term; }
json Logs::Log::getCommand() { return m_command; }

Logs::Logs() {}

void Logs::addLog(int term, const json &command) {
  m_log.push_back(Log(term, command));
}

void Logs::commitLog(int index) { m_commitIndex = index; }
void Logs::applyLog(int index) {
  // TODO Apply command
  m_lastApplied = index;
}

Logs::Log Logs::getLog(int index) { return m_log[index]; }
int Logs::getCommitIndex() { return m_commitIndex; }
int Logs::getLastApplied() { return m_lastApplied; }
