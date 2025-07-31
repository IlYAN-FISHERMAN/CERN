#include "tester.hh"

void fillData(IoMap *map){
	for (size_t i = 0, j = std::abs(rand() % 10); i < j; i++){
		int uid = std::abs(rand() % 4);
		int gid = std::abs(rand() % 4);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map->AddRead(1, "mgm", uid, gid, bytes);
			map->AddWrite(1, "mgm", uid, gid, bytes * 2);
		}
	}
	for (size_t i = 0, j = std::abs(rand() % 10); i < j; i++){
		int uid = std::abs(rand() % 4);
		int gid = std::abs(rand() % 4);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map->AddRead(1, "fdf", uid, gid, bytes);
			map->AddWrite(1, "fdf", uid, gid, bytes * 2);
		}
	}
	for (size_t i = 0, j = std::abs(rand() % 10); i < j; i++){
		int uid = std::abs(rand() % 4);
		int gid = std::abs(rand() % 4);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map->AddRead(1, "miniRT", uid, gid, bytes);
			map->AddWrite(1, "miniRT", uid, gid, bytes * 2);
		}
	}
}

void fillGroup1(IoMap *map){
	map->AddRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->AddRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->AddRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->AddRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->AddRead(1, "eos", 1, 1, std::abs(rand() % 100));
}

void fillGroup2(IoMap *map){
	map->AddRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->AddRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->AddRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->AddRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->AddRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
}

void fillGroup3(IoMap *map){
	map->AddRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->AddRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->AddRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->AddRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->AddRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
}

void prompt(bool &isMultiT, std::string &input){
	while (true){
		if (isMultiT)
			std::cout << "[MultiThreading][IoMap]-> ";
		else
			std::cout << "[SingleThread][IoMap]-> ";
		std::getline(std::cin, input);
		if (input.empty())
			continue;
		break;
	}
}

void print(IoMap *map){
	auto snap = map->GetAllStatsSnapshot();
	for (auto it : snap){
		std::cout << C_GREEN << "┌─[" << C_CYAN << "IoMap" << C_GREEN << "]" << C_RESET;
		std::cout << C_GREEN << "[" << C_CYAN << "id:" << it.first << C_GREEN << "]" << C_RESET;
		std::cout << C_GREEN << "[" <<  C_CYAN << "app:"<< it.second->getApp() << C_GREEN << "]" << C_RESET;
		std::cout << C_GREEN << "[" << C_CYAN << "uid:" << it.second->getUid() << C_GREEN << "]" << C_RESET;
		std::cout << C_GREEN << "[" << C_CYAN << "gid:" << it.second->getGid() << C_GREEN << "]" << C_RESET;
		std::cout << C_GREEN << "[" << C_CYAN << "sR:" << it.second->getSize(IoStat::Marks::READ)
			<< "/sW:"<< it.second->getSize(IoStat::Marks::WRITE) << C_GREEN << "]" << C_RESET;
		std::cout << std::endl << C_GREEN << "└─[" << C_CYAN << "IoStat" << C_GREEN << "]" << C_RESET;
		std::cout << std::fixed << std::setprecision(3) << C_WHITE << it.second << C_RESET << std::endl;
	}
}

void fill(IoMap *map, std::stringstream &os){
	std::string cmd;
	os >> cmd;
	if (cmd.empty()){
		fillData(map);
		std::cout << "fillData" << std::endl;
	}
	else if (cmd == "1")
		fillGroup1(map);
	else if (cmd == "2")
		fillGroup2(map);
	else if (cmd == "3")
		fillGroup3(map);
	else
		std::cerr << "IoMap: " << "fill: @params[1, 2 , 3, NULL]" << std::endl;
}

void purge(bool &isMultiT, IoMap *map){
	if (map){
		delete map;
		if (isMultiT)
			map = new IoMap();
		else
			map = new IoMap(0);
	}
}

int set(IoMap *map, std::stringstream &os){
	std::string params;
	(void)params;
	(void)map;
	(void)os;
	return 0;
}

int execCmd(std::string &input, IoMap *map, bool &isMultiT){
	std::stringstream os(input);
	std::string cmd;
	os >> cmd;
	if (cmd == "exit"){
		std::cout << "exit" << std::endl;
		return 1;
	}
	else if (cmd == "print" || cmd == "p")
		print(map);
	else if (cmd == "fill")
		fill(map, os);
	else if (cmd == "clear" || cmd == "c")
		std::cout << "\033c";
	else if (cmd == "purge")
		purge(isMultiT, map);
	else if (cmd == "set")
			set(map, os);
	else
		std::cerr << "IoMap: " << input << " :command not found" << std::endl;
	if (cmd != "clear" && cmd != "c")
		std::cout << std::endl;
	return 0;
}

int testInteractiveIoMap(){
	IoMap *map;
	bool isMultiT = false;

	while (true){
		std::lock_guard<std::mutex> lock(IoMap::_osMutex);
		std::string tmp;
		std::cout << "run multithreading? [y/n]: ";
		std::getline(std::cin, tmp);
		if (tmp != "y" && tmp != "n")
			continue;
		tmp == "y" ? isMultiT = true : isMultiT = false;
		if (isMultiT)
			map = new IoMap();
		else
			map = new IoMap(0);
		std::cout << "\033c";
		break;
	}
	while(true){
		std::unique_lock<std::mutex> lock(IoMap::_osMutex);
		std::string input;
		if (isMultiT)
			lock.unlock();
		prompt(isMultiT, input);
		if (execCmd(input, map, isMultiT) == 1)
			break ;
	}
	delete map;
	return 0;
}

int testIoMap(){
	std::multimap<std::string, std::optional<std::pair<double, double> > > data;
	std::vector<IoMap*> maps;
	size_t nbrOfMap = 3;

	for (size_t i = 0; i < nbrOfMap; i++)
		maps.push_back(new IoMap());
	for (size_t i = 0; i < nbrOfMap; i++)
		fillData(maps.at(i));

	for (size_t i = 0; i < 5; i++){
		std::lock_guard<std::mutex> lock(IoMap::_osMutex);

		for (auto &it : maps){
			data.insert({"mgm", it->getBandwidth("mgm", IoStat::Marks::READ, 2)});
			data.insert({"mgm", it->getBandwidth("mgm", IoStat::Marks::WRITE, 2)});
			data.insert({"uid_t: 2", it->getBandwidth<uid_t>(2, IoStat::Marks::READ)});
			data.insert({"uid_t: 2", it->getBandwidth<uid_t>(2, IoStat::Marks::WRITE)});
			data.insert({"gid_t: 1", it->getBandwidth<gid_t>(1, IoStat::Marks::READ)});
			data.insert({"gid_t: 1", it->getBandwidth<gid_t>(1, IoStat::Marks::WRITE)});
			for (auto &it : data){
				if (it.second.has_value()){
					std::cout << "map[" << it.first << "]: "
						<< "avrg: " << it.second->first << " | standard deviation: " << it.second->second << std::endl;
				}
				else
					std::cout << "no value" << std::endl;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	return 0;
}
