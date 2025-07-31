#include <gtest/gtest.h>
#include "tester.hh"

TEST(IoStat, CleanData) {
	EXPECT_EQ(testIoStatCleaning(), 0);
}

TEST(IoStat, FillData) {
	EXPECT_EQ(testIoStatFillData(), 0);
}

TEST(IoMap, FillData) {
	EXPECT_EQ(testIoMap(), 0);
}

/// Only for debugging
// TEST(IoMap, testWithInteraction) {
// 	EXPECT_EQ(testInteractiveIoMap(), 0);
// }

int main(int ac, char **av) {
	(void)ac;
	(void)av;

	// testIoMap();
	// testInteractiveIoMap();
	// testIoStatFillData();
	// testIoStatCleaning();
    // ::testing::InitGoogleTest(&ac, av);
    // return RUN_ALL_TESTS();
}
