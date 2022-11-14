#include "mpi_wrappers.hh"

void send(Message::Message &message, int destination_rank) {
  std::string messageString = message.toJSON().dump();
  MPI_Send(messageString.c_str(), messageString.size(), MPI_CHAR,
           destination_rank, message.getType(), MPI_COMM_WORLD);
}

void sendAll(Message::Message &message, int source_rank, int world_size) {
  for (int rank = 0; rank < world_size; rank++) {
    if (rank == source_rank) continue;
    send(message, rank);
  }
}

json recv(MPI_Status &status) {
  int messageSize;
  MPI_Get_count(&status, MPI_CHAR, &messageSize);

  std::vector<char> buffer(messageSize);

  MPI_Recv(buffer.data(), messageSize, MPI_CHAR, status.MPI_SOURCE,
           status.MPI_TAG, MPI_COMM_WORLD, &status);

  std::string messageString(buffer.begin(), buffer.end());

  json messageJson = json::parse(messageString);

  return messageJson;
}

std::optional<MPI_Status> checkForMessage() {
  int request_completed;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request_completed,
             &status);

  if (!request_completed) return std::nullopt;

  return std::optional<MPI_Status>(status);
}
