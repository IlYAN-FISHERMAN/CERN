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

int testIoAggregateMap(){
	IoAggregateMap map;

	fillData(map);
	while (true){
	std::this_thread::sleep_for(std::chrono::seconds(4));
		std::cout << map << std::endl;
	}
	return 0;
}
