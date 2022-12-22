//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_SHIPFIGHTSTATS_HPP_
#define SOURCES_ESD_CORE_DATA_SHIPFIGHTSTATS_HPP_

#include <type_traits>

namespace esd {
struct ShipFightStats {
  char computers;
  char shields;
  char hulls;
  char initiative;
  char cannon_yellow;
  char cannon_orange;
  char cannon_blue;
  char cannon_red;
  char rocket_yellow;
  char rocket_orange;
  char rocket_blue;
  char rocket_red;
};

static_assert(std::is_trivial<ShipFightStats>());

}// namespace esd

#endif//SOURCES_ESD_CORE_DATA_SHIPFIGHTSTATS_HPP_
