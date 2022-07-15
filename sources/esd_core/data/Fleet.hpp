//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_FLEET_HPP_
#define SOURCES_ESD_CORE_DATA_FLEET_HPP_

#include <esd_core/data/ShipFightStats.hpp>

namespace esd {
struct Fleet {
  ShipFightStats ships[4];
  char ships_count;
  Fleet & Add(const ShipFightStats ship)
  {
    ships[ships_count++] = ship;
    return *this;
  }
};

static_assert(std::is_trivial<Fleet>());

}// namespace esd

#endif//SOURCES_ESD_CORE_DATA_FLEET_HPP_
