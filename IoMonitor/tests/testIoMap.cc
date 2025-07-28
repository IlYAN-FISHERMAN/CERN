#include "tester.hh"

int testIoMap(){
	IoMap map(1);

	map.AddRead(42, "mgm", 4, 2, 1);
	map.AddRead(42, "mgm", 4, 2, 532);
	map.AddRead(42, "mgm", 4, 2, 241);
	map.AddRead(42, "mgm", 4, 2, 421);
	map.AddRead(42, "mgm", 4, 2, 721);

	map.AddRead(42, "mgm", 4, 4, 9042);
	map.AddRead(42, "mgm", 4, 4, 9042);
	map.AddRead(42, "mgm", 4, 4, 4290);
	map.AddRead(42, "mgm", 4, 4, 5450);
	map.AddRead(42, "mgm", 4, 4, 540);

	map.AddRead(42, "mgm", 4, 44, 852);
	map.AddRead(42, "mgm", 4, 44, 425);
	map.AddRead(42, "mgm", 4, 44, 312);

	map.AddRead(42, "mgm", 4752, 2342, 70);
	map.AddRead(42, "mgm", 4752, 2342, 7244);

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


	// std::this_thread::sleep_for(std::chrono::seconds(2));
	std::vector<std::optional<std::pair<double, double> > > data;

	data.push_back(map.getBandwidth("mgm", IoStat::Marks::READ, 3));
	data.push_back(map.getBandwidth("pdf", IoStat::Marks::READ, 3));
	data.push_back(map.getBandwidth<uid_t>(10, IoStat::Marks::READ, 3));
	data.push_back(map.getBandwidth<gid_t>(2, IoStat::Marks::READ, 3));
	for (auto it : data){
		if (it.has_value())
			std::cout << "avrg: " << it->first << " | standard deviation: " << it->second << std::endl;
		else
			std::cout << "no value" << std::endl;
		// std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	return 0;
}
