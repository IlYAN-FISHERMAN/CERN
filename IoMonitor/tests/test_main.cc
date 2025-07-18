#include <gtest/gtest.h>
#include "test.hh"

int main(int ac, char **av) {
	(void)ac;
	(void)av;
	testIoStat();
    // ::testing::InitGoogleTest(&argc, argv);
    // return RUN_ALL_TESTS();
}
