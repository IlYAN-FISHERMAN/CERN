#include "tester.hh"

int testIoMap(){
	IoMap map(1);

	map.AddRead(42, "mgm", 4, 2, 10);
	map.AddRead(42, "mgm", 4, 2, 10);
	// auto it = map.getBandwidth("mgm");
	// std::cout << "avrg: " << it->first << " | standard deviation: " << it->second << std::endl;
	auto it = map.getBandwidth<uint64_t>(42, IoStat::Marks::READ);
	std::cout << "avrg: " << it->first << " | standard deviation: " << it->second << std::endl;
	// std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
