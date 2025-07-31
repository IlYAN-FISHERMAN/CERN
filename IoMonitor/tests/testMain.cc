#include <gtest/gtest.h>
#include "tester.hh"

// TEST(IoStat, CleanData) {
// 	EXPECT_EQ(testIoStatCleaning(), 0);
// }
//
// TEST(IoStat, FillData) {
// 	EXPECT_EQ(testIoStatFillData(), 0);
// }
//
int main(int ac, char **av) {
	(void)ac;
	(void)av;

	// testIoMap();
	testInteractiveIoMap();
	// testIoStatFillData();
    // ::testing::InitGoogleTest(&ac, av);
    // return RUN_ALL_TESTS();
}
