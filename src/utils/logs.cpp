#include "logs.hh"

// LOG

Logs::Log::Log(int term, const json &command)
    : m_term(term), m_command(command) {}

int Logs::Log::getTerm() const { return m_term; }
json Logs::Log::getCommand() const { return m_command; }

// LOGS

Logs::Logs() {}

void Logs::addLog(int term, const json &command) {
  m_log.push_back(Log(term, command));
}

void Logs::commitLog(int index) { m_commitIndex = index; }
void Logs::apply() {
  // If commitIndex > lastApplied: increment lastApplied, apply log[lastApplied]
  // to state machine
  while (m_commitIndex > m_lastApplied) {
    m_lastApplied++;
    // TODO Apply command
  }
}

Logs::Log Logs::getLog(int index) {
  if (index < 0 || index >= m_log.size())
    throw std::out_of_range("Index out of range");

  return m_log[index];
}

std::vector<Logs::Log> Logs::getLastLogs(int startIndex) {
  return std::vector<Log>(m_log.begin() + startIndex, m_log.end());
}

int Logs::getCommitIndex() { return m_commitIndex; }

int Logs::getLastApplied() { return m_lastApplied; }

int Logs::getLastIndex() { return m_log.size(); }
