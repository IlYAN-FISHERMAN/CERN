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
	ASSERT_FALSE(testIoMapSummary());
	EXPECT_EQ(testIoMapBigVolume(), 0);
}

/// Only for debugging
// TEST(IoMap, testWithInteraction) {
// 	EXPECT_EQ(testInteractiveIoMap(), 0);
// }

int main(int ac, char **av) {
	int code = 0;
	(void)ac;
	(void)av;
	(void)code;

	// testIoMapSpecificCase();
	// testIoMap();
	// testIoMapSummary() < 0 ? code = -1 : code = 0;
	// testIoAggregateMap() < 0 ? code = -1 : code = 0;
	// testIoMapBigVolume() < 0 ? code = -1 :
	// testIoMapBigVolume() < 0 ? code = -1 :
	// testIoMapBigVolume() < 0 ? code = -1 : code = 0;
	// int code = testIoMapExactValue();
	// int code = testIoStatExactValue();
	testIoAggregateMap() < 0 ? code = -1 : code = 0;
	std::cout << "code: " << code << std::endl;
    // ::testing::InitGoogleTest(&ac, av);
    // return RUN_ALL_TESTS();
}
