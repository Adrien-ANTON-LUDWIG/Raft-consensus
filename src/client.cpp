#include "client.hh"

#include "messages/CMD/load.hh"
#include "messages/mpi_wrappers.hh"

using namespace MessageNS;

Client::Client(int id, int nbServer) : m_id(id), m_nbServer(nbServer) {}

void Client::update() {
  // Create message
  CMD::Load message("test.txt", m_id);

  // Send message
  send(message, m_leaderId);



  // Receive response
  std::optional<MPI_Status> status = checkForMessage();

  if (!status.has_value()) return;

  // if (!response.success) {
    // Look leaderId
    // Save it
    // Send message to the leader
  // }
}
