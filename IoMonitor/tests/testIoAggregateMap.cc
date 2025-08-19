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

template <typename T>
void printSummary(IoAggregateMap &map, size_t winTime, const T index){
	std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: " << winTime << C_GREEN << "]" << C_RESET << std::endl;
	std::cout << map.getSummary(winTime, index) << std::endl;
}
template <typename T>
void printSummary(IoAggregateMap &map, size_t winTime, io::TYPE type, const T index){
	std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: " << winTime << C_GREEN << "]" << C_RESET << std::endl;
	std::cout << map.getSummary(winTime, type, index) << std::endl;
}

int testIoAggregateMap(){
	IoAggregateMap map;
	
	// Add window
	// map.addWindow(60);
	// map.addWindow(120);
	// Window for random with one unique read/write
	// map.addWindow(777);
	// Window track everything
	// map.addWindow(9999);

	// map.addWindow(42);
	// map.addWindow(3);

	// set Tracks
	map.setTrack(60, "eos");
	map.setTrack(120, io::TYPE::UID, 14);
	map.setTrack(777, "mgm");

	// Does not exist
	map.setTrack(60, "dosnotcompile");
	map.setTrack(4242, io::TYPE::GID, 404);
	map.setTrack(404, io::TYPE::GID, 404);

	map.setTrack(3, io::TYPE::GID, 404);


	map.setTrack(9999, "eos");
	map.setTrack(9999, "mgm");
	map.setTrack(9999, io::TYPE::UID, 14);
	map.setTrack(9999, io::TYPE::GID, 3);
	map.setTrack(9999, io::TYPE::GID, 404);

	for (int i = 0; true; i++){
		std::string input;
		std::cout << "[" << i << "]IoMonitor-> ";
		std::getline(std::cin, input);
		if (input == "m")
			std::cout << map << std::endl;
		else if (input == "c")
			std::cout << "\033c";
		else if (input == "exit")
			break ;
		else{
			std::string cmd;
			std::stringstream stream(input);
			if (stream >> cmd){
				int winTime = 0;
				uid_t uid = 0;
				gid_t gid = 0;
				size_t bytes = 0;
				if (cmd == "set" && stream >> winTime){
					stream >> cmd;
					if (cmd == "uid" && stream >> uid)
						map.setTrack(winTime, io::TYPE::UID, uid);
					else if (cmd == "gid" && stream >> gid)
						map.setTrack(winTime, io::TYPE::GID, gid);
					else
						map.setTrack(winTime, cmd);
				}
				else if (cmd == "add" && stream >> winTime)
					map.addWindow(winTime);
				else if (input == "r"){
					int fileId = 0;
					std::string appName;
					if (stream >> fileId >> appName >> uid >> gid >> bytes)
						map.addRead(fileId, appName, uid, gid, bytes);
					else
						std::cout << "add read failed" << std::endl;
				}
				else if (input == "w"){
					int fileId = 0;
					std::string appName;
					if (stream >> fileId >> appName >> uid >> gid >> bytes)
						map.addWrite(fileId, appName, uid, gid, bytes);
					else
						std::cout << "add write failed" << std::endl;
				}
				else if (input == "p"){
					if (stream >> winTime >> cmd){
						if (cmd == "uid" && stream >> uid)
							printSummary(map, winTime, io::TYPE::UID, uid);
						else if (cmd == "gid" && stream >> gid)
							printSummary(map, winTime, io::TYPE::GID, gid);
						else
							printSummary(map, winTime, cmd);
					}
				}
			}
		}
	}
	return 0;
}
