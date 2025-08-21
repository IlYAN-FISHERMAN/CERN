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
	EXPECT_EQ(testIoMapIds(), 0);
}

TEST(IoAggregateMap, exactValue) {
	ASSERT_FALSE(testIoAggregateMapWindow());
	EXPECT_EQ(testIoAggregateMap(), 0);
}

/// Only for debugging, interaction mode with command line
// TEST(IoMap, testWithInteraction) {
// 	EXPECT_EQ(testInteractiveIoMap(), 0);
// }

/// Only for debugging, interaction mode with command line
// TEST(IoMap, testWithInteraction) {
// 	EXPECT_EQ(testIoAggregateMapInteract(), 0);
// }

int main(int ac, char **av) {
	int code = 0;
	(void)ac;
	(void)av;
	(void)code;

	// code = testIoStatExactValue();

	// code = testIoMapSpecificCase();
	// code = testIoMapSummary();
	// code = testIoMapBigVolume();
	// code = testIoMapBigVolume();
	// code = testIoMapBigVolume();
	// code = testIoMapExactValue();
	// code = testIoMapIds();

	// code = testIoAggregateMapWindow();
	// code = testIoAggregateMap();
	// code = testIoAggregateMapInteract();
	// std::cout << "code: " << code << std::endl;
    ::testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
