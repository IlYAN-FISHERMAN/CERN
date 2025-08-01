#include <gtest/gtest.h>
#include "tester.hh"

TEST(IoStat, CleanData) {
	EXPECT_EQ(testIoStatCleaning(), 0);
}

TEST(IoStat, FillData) {
	EXPECT_EQ(testIoStatFillData(), 0);
}

TEST(IoStat, exactValue) {
	EXPECT_EQ(testIoStatExactValue(), 0);
}

TEST(IoMap, FillData) {
	EXPECT_EQ(testIoMapData(), 0);
}

TEST(IoMap, exactValue) {
	ASSERT_FALSE(testIoMapSpecificCase());
	ASSERT_FALSE(testIoMapExactValue());
	EXPECT_EQ(testIoMapBigVolume(), 0);
}

/// Only for debugging
// TEST(IoMap, testWithInteraction) {
// 	EXPECT_EQ(testInteractiveIoMap(), 0);
// }

int main(int ac, char **av) {
	(void)ac;
	(void)av;
	// int code = 0;

	// testIoMapSpecificCase();
	// testIoMap();
	// testIoMapBigVolume() < 0 ? code = -1 :
	// testIoMapBigVolume() < 0 ? code = -1 :
	// testIoMapBigVolume() < 0 ? code = -1 : code = 0;
	// int code = testIoMapExactValue();
	// int code = testIoStatExactValue();
	// testInteractiveIoMap();
	// testIoStatFillData();
	// testIoStatCleaning();
	// std::cout << "code: " << code << std::endl;
    ::testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
