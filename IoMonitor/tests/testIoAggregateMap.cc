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
	std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: "
		<< winTime << C_GREEN << "][" << C_CYAN << "summary of appName: " << std::string(index)
		<< C_GREEN << "]" << C_RESET << std::endl;
	std::cout << map.getSummary(winTime, index) << std::endl;
}
template <typename T>
void printSummary(IoAggregateMap &map, size_t winTime, io::TYPE type, const T index){
	if (type == io::TYPE::UID)
		std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: "
			<< winTime << C_GREEN << "][" << C_CYAN << "summary of uid: " << index
			<< C_GREEN << "]" << C_RESET << std::endl;
	if (type == io::TYPE::GID)
		std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: "
			<< winTime << C_GREEN << "][" << C_CYAN << "summary of gid: " << index
			<< C_GREEN << "]" << C_RESET << std::endl;
	else
		return;
	std::cout << map.getSummary(winTime, type, index) << std::endl;
}


int testIoAggregateMap(){
	IoAggregateMap map;
	
	// Add window
	map.addWindow(3600);

	// set Tracks
	map.setTrack(3600, "eos");
	map.setTrack(3600, "fdf");
	map.setTrack(3600, "mgm");
	map.setTrack(3600, io::TYPE::UID, 12);
	map.setTrack(3600, io::TYPE::GID, 11);

	for (size_t i = 0; i < 25; i++){
		map.addWrite(1, "eos", 12, 11, std::abs(rand())%10000);
		map.addWrite(1, "eos", 1, 11, std::abs(rand())%10000);
		map.addWrite(1, "mgm", 1, 11, std::abs(rand())%10000);
		map.addWrite(1, "fdf", 12, 1, std::abs(rand())%10000);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	auto eos = map.getSummary(3600, "eos");
	auto mgm = map.getSummary(3600, "mgm");
	auto fdf = map.getSummary(3600, "fdf");
	auto uid = map.getSummary(3600, io::TYPE::UID, 12);
	auto gid = map.getSummary(3600, io::TYPE::GID, 11);

	if (!eos.has_value() || !mgm.has_value() || !fdf.has_value() ||
		!uid.has_value() || !gid.has_value())
		return -1;
	return 0;
}

int testIoAggregateMapInteract(){
	IoAggregateMap map;

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
				else if (cmd == "r"){
					int fileId = 0;
					std::string appName;
					if (stream >> fileId >> appName >> uid >> gid >> bytes)
						map.addRead(fileId, appName, uid, gid, bytes);
					else
						std::cout << "add read failed" << std::endl;
				}
				else if (cmd == "w"){
					int fileId = 0;
					std::string appName;
					if (stream >> fileId >> appName >> uid >> gid >> bytes)
						map.addWrite(fileId, appName, uid, gid, bytes);
					else
						std::cout << "add write failed" << std::endl;
				}
				else if (cmd == "p"){
					if (stream >> winTime >> cmd){
						if (cmd == "uid" && stream >> uid)
							printSummary(map, winTime, io::TYPE::UID, uid);
						else if (cmd == "gid" && stream >> gid)
							printSummary(map, winTime, io::TYPE::GID, gid);
						else
							printSummary(map, winTime, cmd);
					}
					else
						std::cout << "print Summary failed" << std::endl;
				}
				else if (cmd == "s"){
					size_t index = 0;
					if (stream >> winTime){
						if (stream >> index)
							map.shiftWindow(winTime, index);
						else
							map.shiftWindow(winTime);
					}
				}
				else
					std::cout << "Monitor: command not found: " << input << std::endl;
			}
		}
	}
	return 0;
}
