//
// Created by aypahyo on 15.12.22.
//

#ifndef SOURCES_ESD_CORE_DATA_SHIPBUILDER_HPP_
#define SOURCES_ESD_CORE_DATA_SHIPBUILDER_HPP_

#include <esd_core/data/ShipFightStats.hpp>
#include <functional>


namespace esd {
struct ShipBuilder {
  ShipFightStats construction_stats{};

  ShipBuilder& w( std::function<void(ShipFightStats*)> ship_part )
  {
    ship_part(&construction_stats);
    return *this;
  }

  ShipFightStats stats()
  {
    return construction_stats;
  }

};
}// namespace esd

#endif//SOURCES_ESD_CORE_DATA_SHIPBUILDER_HPP_
