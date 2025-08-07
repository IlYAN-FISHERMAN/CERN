#include "tester.hh"

int testIoAggregateMap(){
	IoAggregateMap map(1);
	size_t nbrOfMap = 3;

	IoMark begin;
	for (size_t i = 0; i < nbrOfMap; i++)
		fillData(&map.getIoMap());

	std::this_thread::sleep_for(std::chrono::seconds(7));
	auto tmp = map.getIoMap().GetAllStatsSnapshot();
	for (auto it : map)
		std::cout << *it.second.get() << std::endl;
	return 0;
}
