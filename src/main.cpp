#include <mpi.h>

#include <chrono>
#include <iostream>

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

  bool worker_alive = true;
  int term = 0;
  int vote_count = 0;
  int vote_granted = false;
  int voted_for = -1;
  auto election_timeout = std::chrono::milliseconds(std::rand() % 15000 + 150);
  if (my_rank == 0) election_timeout = std::chrono::milliseconds(100);
  auto heartbeat_timeout = std::chrono::milliseconds(50);
  auto start_time = std::chrono::system_clock::now();
  auto current_time = std::chrono::system_clock::now();

  int message = -1;
  MPI_Status status;
  MPI_Request request = MPI_REQUEST_NULL;
  bool requested = false;

  STATE state = FOLLOWER;

  // Print out information about MPI_COMM_WORLD
  std::cout << "World Size: " << world_size << "   Rank: " << my_rank
            << "  Election timeout: " << election_timeout.count() << std::endl;

  while (worker_alive) {
    current_time = std::chrono::system_clock::now();

    if (state == LEADER) {
      if (current_time - start_time >= heartbeat_timeout) {
        // MPI_Send("Je suis encore en vie.");
        // MPI_Bcast(&voted_for, 1, MPI_INT, my_rank, MPI_COMM_WORLD);
        printf("Heartbeat send\n");
        start_time = std::chrono::system_clock::now();
      }
    }

    else if (state == FOLLOWER) {
      if (current_time - start_time >= election_timeout) {
        state = CANDIDATE;
        term += 1;
        vote_count += 1;
        voted_for = my_rank;
        vote_granted = true;

        // Send Request vote messages

        std::cout << "Send vote request from " << my_rank << std::endl;
        MPI_Bcast(&voted_for, 1, MPI_INT, my_rank, MPI_COMM_WORLD);
      } else {
        if (!requested && my_rank != 0) {
          printf("Listen for message\n");
          fflush(stdout);
          MPI_Irecv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                    MPI_COMM_WORLD, &request);
          requested = true;
        } else {
          int flag = 0;
          MPI_Test(&request, &flag, &status);

          if (flag) {
            std::cout << "Message received, responding..." << std::endl;

            if (!vote_granted) {
              vote_granted = true;
              voted_for = message;
            }
            // MPI_Send(&voted_for, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG,
                    //  MPI_COMM_WORLD);
          }
        }
      }
    }

    else if (state == CANDIDATE) {
      //   while (election_timeout > 0) {
      //     MPI_Recv(..., timeout) = election_timeout);
      //     // Update vote_count if needed
      //   }
      //   if (vote_count > world_size / 2) state = LEADER;
      //   // Send something
      //   else
      //     state = FOLLOWER;

      //   election_timeout = std::chrono::milliseconds(std::rand() % 150 +
      //   150); start_time = std::chrono::system_clock::now();
    }
  }

  /////////////////////////////////////////////////////////////////

  // Finalize MPI
  // This must always be called after all other MPI functions
  MPI_Finalize();

  return 0;
}
