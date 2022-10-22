#include <mpi.h>

#include <chrono>
#include <iostream>
#include <thread>

enum STATE { FOLLOWER, CANDIDATE, LEADER };

int main(int argc, char **argv) {
  // Initialize MPI
  // This must always be called before any other MPI functions
  MPI_Init(&argc, &argv);

  // Get the number of processes in MPI_COMM_WORLD
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of this process in MPI_COMM_WORLD
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /////////////////////////////////////////////////////////////////
  std::srand(my_rank);
  bool worker_alive = true;
  int term = 0;
  int vote_count = 0;
  int vote_granted = false;
  int voted_for = -1;
  auto election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
  if (my_rank == 0) election_timeout = std::chrono::milliseconds(100);
  auto heartbeat_timeout = std::chrono::milliseconds(50);
  auto start_time = std::chrono::system_clock::now();
  auto current_time = std::chrono::system_clock::now();

  int message = -1;
  MPI_Status status;
  MPI_Request request = MPI_REQUEST_NULL;

  STATE state = FOLLOWER;

  // Print out information about MPI_COMM_WORLD
  std::cout << my_rank << ": Election timeout: " << election_timeout.count()
            << std::endl;

  while (worker_alive) {
    current_time = std::chrono::system_clock::now();

    if (state == LEADER) {
      if (current_time - start_time >= heartbeat_timeout) {
        start_time = std::chrono::system_clock::now();

        // std::cout << my_rank << ": Heartbeat send" << std::endl;

        for (int rank = 0; rank < world_size; rank++) {
          if (rank == my_rank) continue;
          MPI_Send(&voted_for, 1, MPI_INT, rank, 51, MPI_COMM_WORLD);
        }
      }
    } else if (state == FOLLOWER) {
      if (current_time - start_time >= election_timeout) {
        if (request != MPI_REQUEST_NULL) MPI_Cancel(&request);

        state = CANDIDATE;
        term += 1;
        vote_count += 1;
        voted_for = my_rank;
        vote_granted = true;

        // Send Request vote messages
        std::cout << my_rank << ": Send vote request" << std::endl;
        for (int rank = 0; rank < world_size; rank++) {
          if (rank == my_rank) continue;
          MPI_Send(&voted_for, 1, MPI_INT, rank, 42, MPI_COMM_WORLD);
        }

      } else {
        if (request == MPI_REQUEST_NULL) {
          // std::cout << my_rank << ": Listen for message" << std::endl;
          MPI_Irecv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                    MPI_COMM_WORLD, &request);
        } else {
          int request_completed = 0;
          MPI_Test(&request, &request_completed, &status);

          if (request_completed) {
            // Reset timeout
            start_time = std::chrono::system_clock::now();

            if (status.MPI_TAG == 42) {
              std::cout << my_rank << ": Received vote request from "
                        << status.MPI_SOURCE << " responding..." << std::endl;

              if (!vote_granted) {
                vote_granted = true;
                voted_for = message;
              }

              MPI_Send(&voted_for, 1, MPI_INT, status.MPI_SOURCE,
                       status.MPI_TAG, MPI_COMM_WORLD);
            }
          }
        }
      }
    }

    else if (state == CANDIDATE) {
      while (vote_count <= world_size / 2) {
        MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &status);

        if (message == my_rank) vote_count++;
      }
      if (vote_count > world_size / 2) {
        state = LEADER;
        std::cout << my_rank << ": is the new Leader" << std::endl;
        // Send something
      } else
        state = FOLLOWER;

      election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
      start_time = std::chrono::system_clock::now();
    }
  }

  /////////////////////////////////////////////////////////////////

  // Finalize MPI
  // This must always be called after all other MPI functions
  MPI_Finalize();

  return 0;
}
