#pragma once

#include <mpi.h>

#include <chrono>
#include <iostream>

enum STATE { FOLLOWER, CANDIDATE, LEADER };

class Server {
 public:
  Server(int id, int world_size) {
    this->id = id;
    this->world_size = world_size;

    std::srand(id);
    election_timeout = std::chrono::milliseconds(std::rand() % 150 + 150);
    if (id == 0) election_timeout = std::chrono::milliseconds(100);
    heartbeat_timeout = std::chrono::milliseconds(50);
    start_time = std::chrono::system_clock::now();

    std::cout << id << ": Election timeout: " << election_timeout.count()
              << std::endl;
  }

  void followerUpdate();
  void candidateUpdate();
  void leaderUpdate();
  void update();

  /**
   * Variables
   */

  int id;  // Corresponding to MPI rank

  STATE state = FOLLOWER;
  int term = 0;
  int vote_count = -1;
  int voted_for = -1;

  // MPI management
  int world_size;
  int message;
  MPI_Request request = MPI_REQUEST_NULL;

  // Time management
  std::chrono::milliseconds election_timeout;
  std::chrono::milliseconds heartbeat_timeout;
  std::chrono::_V2::system_clock::time_point start_time;
  std::chrono::_V2::system_clock::time_point current_time;
};
