//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_COMBAT_DICERESULTS_HPP_
#define SOURCES_ESD_CORE_COMBAT_DICERESULTS_HPP_

namespace esd {
struct DiceResults {
  ////44 dice is the maximum dice one ship type could have 4 Cruisers with one engine, 4 yellow double rockets and the ancient yellow triple rocket = 4 * 11 = 44 dice
  char results[44];
  char yellow_results(int index){ return results[index]; }
  char yellow_results_count;
  char orange_results(int index){ return results[yellow_results_count + index]; }
  char orange_results_count;
  char blue_results(int index){ return results[yellow_results_count + orange_results_count + index]; }
  char blue_results_count;
  char red_results(int index){ return results[yellow_results_count + orange_results_count + blue_results_count + index]; }
  char red_results_count;
};

static_assert(std::is_trivial<DiceResults>());

}// namespace esd
#endif//SOURCES_ESD_CORE_COMBAT_DICERESULTS_HPP_
