//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_SHIPFIGHTSTATS_HPP_
#define SOURCES_ESD_CORE_DATA_SHIPFIGHTSTATS_HPP_

#include <esd_core/data/ShipPart.hpp>

namespace esd {
struct ShipFightStats {
  char computers;
  char shields;
  char hulls;
  char initiative;
  char yellow_cannon;
  char yellow_rocket;
  char orange_cannon;
  char orange_rocket;
  char blue_cannon;
  char blue_rocket;
  char red_cannon;
  char red_rocket;
  ShipFightStats & Add(const ShipPart & p)
  {
    computers += p.computers;
    shields += p.shields;
    hulls += p.hulls;
    initiative += p.initiative;
    yellow_cannon += p.yellow_cannon;
    yellow_rocket += p.yellow_rocket;
    orange_cannon += p.orange_cannon;
    orange_rocket += p.orange_rocket;
    blue_cannon += p.blue_cannon;
    blue_rocket += p.blue_rocket;
    red_cannon += p.red_cannon;
    red_rocket += p.red_rocket;
    return *this;
  }
};

static_assert(std::is_trivial<ShipFightStats>());

}// namespace esd

#endif//SOURCES_ESD_CORE_DATA_SHIPFIGHTSTATS_HPP_
