#include "worlds_info.hh"

std::ostream& operator<<(std::ostream& os, const WorldInfo& wi) {
  os << "{ com: " << wi.com << ", rank: " << wi.rank << ", world_size: " << wi.world_size << " }";

  return os;
}

std::ostream& operator<<(std::ostream& os, const Universe& u) {
  os << "Server world: " << u.serverWorld << "\n";
  os << "Client-server world: " << u.clientServerWorld << "\n";
  os << "REPL world: " << u.replWorld;

  return os;
}