//
// Created by aypahyo on 09.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_SHIPPART_HPP_
#define SOURCES_ESD_CORE_DATA_SHIPPART_HPP_

#include <type_traits>

namespace esd {
struct ShipPart {
  char computers;
  char shields;
  char hulls;
  char drives;
  char energy;
  char initiative;
  char space;
  char yellow_cannon;
  char yellow_rocket;
  char orange_cannon;
  char orange_rocket;
  char blue_cannon;
  char blue_rocket;
  char red_cannon;
  char red_rocket;
};

static_assert(std::is_trivial<ShipPart>());

}// namespace esd

#endif//SOURCES_ESD_CORE_DATA_SHIPPART_HPP_
