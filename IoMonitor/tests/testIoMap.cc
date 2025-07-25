#include "tester.hh"

int testIoMap(){
	IoMap map(1);

	map.AddRead(42, "mgm", 4, 2, 1004);
	map.AddRead(42, "mgm", 4, 2, 0436);

	map.AddRead(42, "pdf", 4, 2, 6340);
	map.AddRead(42, "pdf", 4, 2, 5);

	map.AddRead(42, "pdf", 10, 2, 100);
	map.AddRead(42, "pdf", 10, 2, 200);
	map.AddRead(42, "pdf", 10, 2, 654);
	map.AddRead(42, "pdf", 10, 2, 245);
	map.AddRead(42, "pdf", 10, 2, 182);

	map.AddRead(42, "pdf", 10, 500, 400);
	map.AddRead(42, "pdf", 10, 500, 500);

	map.AddRead(42, "pdf", 4, 1, 6340);

	map.AddRead(24, "pdf", 10, 2, 100);
	map.AddRead(24, "pdf", 10, 2, 200);
	map.AddRead(24, "pdf", 10, 2, 654);
	map.AddRead(24, "pdf", 10, 2, 245);
	map.AddRead(24, "pdf", 10, 2, 182);

	map.AddRead(50, "pdf", 10, 2, 424);
	map.AddRead(50, "pdf", 10, 2, 28355);
	map.AddRead(50, "pdf", 52, 2, 325);
	map.AddRead(50, "pdf", 52, 2, 985);
	map.AddRead(50, "pdf", 52, 2, 4562);


	// auto it = map.getBandwidth("mgm");
	auto it = map.getBandwidth("pdf", IoStat::Marks::READ);
	(void)it;
	// if (it.has_value())
	// 	std::cout << "avrg: " << it->first << " | standard deviation: " << it->second << std::endl;
	// else
	// 	std::cout << "no value" << std::endl;
	// std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
