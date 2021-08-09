//
// Created by aypahyo on 09.08.21.
//
#include <gtest/gtest.h>
#include <esd_core/samplecore.hpp>

TEST(demosuite, demotest) {
  auto res = esd::sample().getValue();
  ASSERT_EQ(4, res);
}
