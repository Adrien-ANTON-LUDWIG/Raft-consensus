#include "mpi_wrappers.hh"

using namespace MessageNS;

void send(const Message &message, int destination_rank, MPI_Comm channel) {
  std::string messageString = message.toJSON().dump();
  MPI_Send(messageString.c_str(), messageString.size(), MPI_CHAR,
           destination_rank, message.getType(), channel);
}

void sendAll(const Message &message, int source_rank, int world_size, MPI_Comm channel) {
  for (int rank = 0; rank < world_size; rank++) {
    if (rank == source_rank) continue;
    send(message, rank, channel);
  }
}

json recv(MPI_Status &status, MPI_Comm channel) {
  int messageSize;
  MPI_Get_count(&status, MPI_CHAR, &messageSize);

  std::vector<char> buffer(messageSize);

  MPI_Recv(buffer.data(), messageSize, MPI_CHAR, status.MPI_SOURCE,
           status.MPI_TAG, channel, &status);

  std::string messageString(buffer.begin(), buffer.end());

  json messageJson = json::parse(messageString);

  return messageJson;
}

std::optional<MPI_Status> checkForMessage(MPI_Comm channel, int source) {
  int request_completed;
  MPI_Status status;
  MPI_Iprobe(source, MPI_ANY_TAG, channel, &request_completed,
             &status);

  if (!request_completed) return std::nullopt;

  return std::optional<MPI_Status>(status);
}

json waitForResponse(int source, MPI_Comm channel) {
  std::optional<MPI_Status> statusOpt;
  do
  {
    statusOpt = checkForMessage(channel, source);
  } while (!statusOpt.has_value());
  
  return recv(statusOpt.value(), channel);
}