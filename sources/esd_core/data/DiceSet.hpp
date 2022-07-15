//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_DICESET_HPP_
#define SOURCES_ESD_CORE_DATA_DICESET_HPP_

#include <type_traits>

namespace esd {
struct DiceSet {
  char yellow;
  char orange;
  char blue;
  char red;
  char total() { return yellow + orange + blue + red; }
};

static_assert(std::is_trivial<DiceSet>());
}
#endif//SOURCES_ESD_CORE_DATA_DICESET_HPP_
