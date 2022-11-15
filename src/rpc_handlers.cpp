#include "messages/CMD/load.hh"
#include "messages/CMD/loadResponse.hh"
#include "server.h"

using namespace MessageNS;

#define TO_IMPLEMENT 42

void Server::handleLoad(const json& json) {
  CMD::Load load(json);

  // TODO File management

  // TODO Replace with true file UID
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  uuids::uuid fileUID = gen();

  CMD::LoadResponse response(fileUID, TO_IMPLEMENT, true,
                             m_id);  // TODO replace true by success check
  send(response, load.getOriginId());
}