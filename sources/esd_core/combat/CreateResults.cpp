//
// Created by aypahyo on 10.08.21.
//

#include "CreateResults.hpp"
#include <iostream>
#include <limits>

using namespace esd;

long CreateResults::GetTotalOutcomes(int dice){
  long ret = 1;
  for (int i = 0; i < dice; ++i) { ret *= 6; }
  return ret;
}

std::vector<DiceResults> CreateResults::GetAllRolls(int yellow, int orange, int blue, int red) {
  std::vector<DiceResults> vec;
  auto s_max = GetTotalOutcomes(yellow + orange + blue + red);
  try {
    vec.reserve(s_max);
  }
  catch (const std::bad_alloc & e)
  {
    std::cout << e.what() << "\n";
    auto mb_size = sizeof(DiceResults) * s_max / 1024.0 /1024.0 /1024.0 /1024.0;
    std::cout << "requested " << mb_size << " TB of memory\n";
    std::cout << "requested " << s_max << " elements\n";
    std::cout << "vec max " << std::numeric_limits<decltype(vec)::size_type>::max() << " elements\n";
    std::cout << "consider using a different api\n";
    return vec;
  }
  for (int sequence = 0; sequence < s_max; ++sequence) {
    DiceResults d;
    d = getOutcome(yellow, orange, blue, red, sequence);
    vec.push_back(d);
  }
  return vec;
}

DiceResults CreateResults::getOutcome(int yellow, int orange, int blue, int red, long sequence) {
  DiceResults d;
  auto results = 0;
  d.yellow_results_count = 0;
  d.orange_results_count = 0;
  d.blue_results_count = 0;
  d.red_results_count = 0;

  for(int i = yellow; i > 0; --i) {
    d.results[results++] = (sequence % 6) + 1;
    d.yellow_results_count++;
    sequence /= 6;
  }
  for(int i = orange; i > 0; --i) {
    d.results[results++] = (sequence % 6) + 1;
      d.orange_results_count++;
      sequence /= 6;
  }
  for(int i = blue; i > 0; --i) {
    d.results[results++] = (sequence % 6) + 1;
      d.blue_results_count++;
      sequence /= 6;
  }
  for(int i = red; i > 0; --i) {
    d.results[results++] = (sequence % 6) + 1;
    sequence /= 6;
  }
  return d;
}
