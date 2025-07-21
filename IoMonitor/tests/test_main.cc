#include <gtest/gtest.h>
#include "test.hh"

TEST(IoStat, IoStatBenchmark) {
	EXPECT_EQ(testIoStat(), 0);
}

int main(int ac, char **av) {
    ::testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
