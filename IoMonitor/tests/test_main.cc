#include <gtest/gtest.h>
#include "test.hh"

int main(int ac, char **av) {
	(void)ac;
	(void)av;
	testIoStat();
    // ::testing::InitGoogleTest(&ac, av);
    // return RUN_ALL_TESTS();
}
