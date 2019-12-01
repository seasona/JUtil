#include "gtest/gtest.h"

TEST(gtest, test1){
    int x=11;
    const char* str= "gtest";
    EXPECT_EQ(x, 11);
    EXPECT_STREQ(str, "gtest");
    EXPECT_EQ(x, 10);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}