// ============================================================
//  Sanity Test — Day 2
//  Verify gtest setup is working
// ============================================================

#include <gtest/gtest.h>

TEST(SanityTest, BasicAssertion) {
    EXPECT_EQ(2 + 2, 4);
    EXPECT_TRUE(true);
    EXPECT_STREQ("5g-awanish", "5g-awanish");
}

TEST(SanityTest, BuildSystemWorks) {
    // Agar ye test pass hota hai → CMake + gtest setup OK
    SUCCEED() << "Day 2 build system is working!";
}
