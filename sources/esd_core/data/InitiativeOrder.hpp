//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_INITIATIVEORDER_HPP_
#define SOURCES_ESD_CORE_DATA_INITIATIVEORDER_HPP_

namespace esd {

constexpr char InitiativeOrder_None = 0;
constexpr char InitiativeOrder_Defender = 5;
constexpr char InitiativeOrder_Attacker = 1;

struct InitiativeOrder {
  char turn_order[8];
};

static_assert(std::is_trivial<InitiativeOrder>());

}// namespace esd

#endif//SOURCES_ESD_CORE_DATA_INITIATIVEORDER_HPP_
