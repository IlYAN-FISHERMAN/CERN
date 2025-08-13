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
	gid_t grps = 3;
	uid_t user = 14;
	int nbrOfBin = 10;

	map.addRead(1, "eos", 14, 3, 5025);
	map.addRead(1, "eos", 14, 3, 425);
	map.addRead(1, "eos", 14, 3, 54225);

	map.addWindow(60, 3, nbrOfBin);
	map.addWindow(120, 3, nbrOfBin);

	if (map.containe(60)){
		map.setTrack(60, "eos");
		map.setTrack(60, io::TYPE::UID, user);
		map.setTrack(60, io::TYPE::GID, grps);
	}

	if (map.containe(120)){
		map.setTrack(120, "eos");
		map.setTrack(120, io::TYPE::UID, user);
		map.setTrack(120, io::TYPE::GID, grps);
	}

	for (int i = 0; true; i++){
		std::string input;
		std::getline(std::cin, input);
		std::cout << i << " " << map << std::endl;
	}
	// std::cout << map << std::endl;
	// std::cout << map.getIoMap() << std::endl;
	return 0;
}

int testIoAggregateMap(){
	return 0;
}
