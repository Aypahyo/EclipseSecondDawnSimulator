//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_COMBAT_COMBATSTATEAGGREGATOR_HPP_
#define SOURCES_ESD_CORE_COMBAT_COMBATSTATEAGGREGATOR_HPP_

#include <esd_core/data/CombatState.hpp>
#include <vector>
namespace esd {
struct CombatStateAggregator {
  std::vector<esd::CombatState> states;
  void Add(esd::CombatState state) {
    auto f = std::find_if(states.begin(), states.end(), [state](const esd::CombatState &other) { return state.Equals(other); });
    if ( f != states.end()) {
      f->outcomes += state.outcomes;
    } else {
      states.push_back(state);
    }
  }
};
}// namespace esd
#endif//SOURCES_ESD_CORE_COMBAT_COMBATSTATEAGGREGATOR_HPP_
