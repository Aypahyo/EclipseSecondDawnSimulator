//
// Created by aypahyo on 09.08.21.
//
#include <gtest/gtest.h>
#include <esd_core/data/ShipParts.hpp>

TEST(ship_parts_data, content_test) {

  using namespace esd;
  ASSERT_EQ(default_hull.hulls, 1);
  ASSERT_EQ(default_nuclear_drive.drives, 1);
  ASSERT_EQ(default_nuclear_drive.energy, -1);
  ASSERT_EQ(default_nuclear_drive.initiative, 1);
  ASSERT_EQ(default_nuclear_source.energy, 3);
  ASSERT_EQ(default_ion_cannon.yellow_cannon, 1);
  ASSERT_EQ(military_plasma_cannon.orange_cannon, 1);
  ASSERT_EQ(rare_soliton_cannon.blue_cannon, 1);
  ASSERT_EQ(grid_antimatter_cannon.red_cannon, 1);
}
