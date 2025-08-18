#include "tester.hh"

void fillData(IoAggregateMap &map){
	for (size_t i = 0; i < 10; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map.addRead(i, "mgm", uid, gid, bytes);
			map.addWrite(i, "mgm", uid, gid, bytes * 3);
		}
	}
	for (size_t i = 10; i < 20; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map.addRead(i, "fdf", uid, gid, bytes);
			map.addWrite(i, "fdf", uid, gid, bytes * 4);
		}
	}
	for (size_t i = 20; i < 30; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map.addRead(i, "miniRT", uid, gid, bytes);
			map.addWrite(i, "miniRT", uid, gid, bytes * 9);
		}
	}
}

int testIoAggregateMapWindow(){
	IoAggregateMap map;

	map.addWindow(60);
	map.addWindow(120);
	map.addWindow(31);
	map.addWindow(9999);
	map.addWindow(9999);
	map.addWindow(9999);
	map.addWindow(9999);
	map.addWindow(1);
	map.addWindow(0);

	auto tmp = map.getAvailableWindows();
	if (!tmp.has_value())
		return -1;

	auto windo = tmp.value();
	if (windo.size() != 4)
		return -1;
	if ((std::find(windo.begin(), windo.end(), 9999) == windo.end())
	|| std::find(windo.begin(), windo.end(), 60) == windo.end()
	|| std::find(windo.begin(), windo.end(), 120) == windo.end()
	|| std::find(windo.begin(), windo.end(), 31) == windo.end())
		return -1;
	if (!map.containe(9999)
		|| !map.containe(120)
		|| !map.containe(31)
		|| !map.containe(60))
		return -1;
	if (map.containe(422425))
		return -1;
	return 0;
}

int testIoAggregateMap(){
	IoAggregateMap map;
	
	// Add window
	map.addWindow(60);
	map.addWindow(120);

	// set Tracks
	map.setTrack(60, "eos");
	map.setTrack(120, io::TYPE::UID, 14);

	// Does not exist


	// Add input
	map.addRead(1, "eos", 14, 3, 5025);
	map.addRead(1, "eos", 14, 3, 425);
	map.addRead(1, "eos", 14, 3, 54225);

	map.addRead(1, "eos", 14, 404, 1050);
	map.addRead(1, "eos", 14, 404, 500);
	map.addRead(1, "eos", 14, 404, 425);

	for (int i = 0; true; i++){
		std::string input;
		map.addRead(1, "eos", 14, 3, 5025 * i);
		map.addRead(1, "eos", 14, 3, 425 * i);
		map.addRead(1, "eos", 14, 3, 54225 - (i * 100));
		map.addRead(1, "eos", 14, 404, 1050 + i);
		map.addRead(1, "eos", 14, 404, 500 * i);
		map.addRead(1, "eos", 14, 404, 425);

		std::cout << "[enter: " << i  << "]"<< std::endl;
		std::getline(std::cin, input);
		std::cout << "\33c";
		// auto summary = map.getSummary(60, "eos");
		// std::cout << summary << std::endl;
		auto summary2 = map.getSummary<gid_t>(120, io::TYPE::UID, 14);
		std::cout << summary2 << std::endl;
	}
	return 0;
}
