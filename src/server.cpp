#include "server.h"

void Server::followerUpdate() {
  if (current_time - start_time >= election_timeout) {
    if (request != MPI_REQUEST_NULL) MPI_Cancel(&request);

    state = CANDIDATE;
    term += 1;
    vote_count += 1;
    voted_for = id;

    // Send Request vote messages
    std::cout << id << ": Send vote request" << std::endl;
    for (int rank = 0; rank < world_size; rank++) {
      if (rank == id) continue;
      MPI_Send(&voted_for, 1, MPI_INT, rank, 42, MPI_COMM_WORLD);
    }

  } else {
    if (request == MPI_REQUEST_NULL) {
      // std::cout << my_rank << ": Listen for message" << std::endl;
      MPI_Irecv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                MPI_COMM_WORLD, &request);
    } else {
      int request_completed = 0;
      MPI_Status status;

      MPI_Test(&request, &request_completed, &status);

      if (request_completed) {
        // Reset timeout
        start_time = std::chrono::system_clock::now();

        if (status.MPI_TAG == 42) {
          std::cout << id << ": Received vote request from "
                    << status.MPI_SOURCE << " responding..." << std::endl;

          if (voted_for == -1) voted_for = message;

          MPI_Send(&voted_for, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG,
                   MPI_COMM_WORLD);
        }
      }
    }
  }
}

void Server::candidateUpdate() {
  while (vote_count <= world_size / 2) {
    MPI_Status status;
    MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
             &status);

    if (message == id) vote_count++;
  }
  if (vote_count > world_size / 2) {
    state = LEADER;
    std::cout << id << ": is the new Leader" << std::endl;
    // Send something
  } else
    state = FOLLOWER;

  election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
  start_time = std::chrono::system_clock::now();
}

void Server::leaderUpdate() {
  if (current_time - start_time >= heartbeat_timeout) {
    start_time = std::chrono::system_clock::now();

    // std::cout << my_rank << ": Heartbeat send" << std::endl;

    for (int rank = 0; rank < world_size; rank++) {
      if (rank == id) continue;
      MPI_Send(&voted_for, 1, MPI_INT, rank, 51, MPI_COMM_WORLD);
    }
  }
}

void Server::update() {
  current_time = std::chrono::system_clock::now();

  switch (state) {
    case LEADER:
      leaderUpdate();
      break;
    case CANDIDATE:
      candidateUpdate();
      break;
    default:  // FOLLOWER
      followerUpdate();
      break;
  }
}