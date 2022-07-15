//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_COMBAT_CREATERESULTS_HPP_
#define SOURCES_ESD_CORE_COMBAT_CREATERESULTS_HPP_

#include <vector>
#include <esd_core/combat/DiceResults.hpp>

namespace esd {

struct CreateResults {
  ////returns dice⁶
  static long GetTotalOutcomes(int dice);
  ////This API is for small sample batches in testing "dice⁶ * sizeof(DiceResult)" is too much memory for serious cases
  static std::vector<DiceResults> GetAllRolls(int yellow, int orange, int blue, int red);
  ////This API has constant memory footprint use it from 0 to dice⁶-1
  static DiceResults getOutcome(int yellow, int orange, int blue, int red, long sequence);
};
}// namespace esd
#endif//SOURCES_ESD_CORE_COMBAT_CREATERESULTS_HPP_
