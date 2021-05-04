#include "mpi_bandwidth_util.hpp"
#include <gtest/gtest.h>

TEST(pair_src_dest, smoke_test) {
  int src, dest;

  pair_src_dest(0, 2, &src, &dest);
  EXPECT_EQ(src, 1);
  EXPECT_EQ(dest, 1);

  pair_src_dest(1, 2, &src, &dest);
  EXPECT_EQ(src, 0);
  EXPECT_EQ(dest, 0);

  pair_src_dest(0, 4, &src, &dest);
  EXPECT_EQ(src, 2);
  EXPECT_EQ(dest, 2);

  pair_src_dest(1, 4, &src, &dest);
  EXPECT_EQ(src, 3);
  EXPECT_EQ(dest, 3);

  pair_src_dest(2, 4, &src, &dest);
  EXPECT_EQ(src, 0);
  EXPECT_EQ(dest, 0);

  pair_src_dest(3, 4, &src, &dest);
  EXPECT_EQ(src, 1);
  EXPECT_EQ(dest, 1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
