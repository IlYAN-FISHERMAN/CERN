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

	map.addWindow(60, 3, 42);
	map.addWindow(120, 3, 13);
	map.addWindow(31, 3, 13);
	map.addWindow(9999, 0, 1);
	map.addWindow(9999, 4, 1);
	map.addWindow(9999, 1, 1);
	map.addWindow(9999, 9999, 1);
	map.addWindow(1, 1, 1);
	map.addWindow(0, 0, 0);

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

	map.addRead(1, "eos", 14, 3, 5025);
	map.addRead(1, "eos", 14, 3, 425);
	map.addRead(1, "eos", 14, 3, 54225);

	map.addWindow(60, 3, 42);
	map.addWindow(120, 3, 13);

	map.setTrack(60, "eos");
	map.setTrack(60, io::TYPE::UID, 14);
	map.setTrack(60, io::TYPE::GID, 3);

	if (map.containe(120)){
		map.setTrack(120, "eos");
		map.setTrack(120, io::TYPE::UID, 14);
		map.setTrack(120, io::TYPE::GID, 3);
	}

	for (int i = 0; true; i++){
		std::string input;
		std::getline(std::cin, input);
		// std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << i << " " << map << std::endl;
	}
	return 0;
}
