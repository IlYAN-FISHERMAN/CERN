#include "tester.hh"

void fillData(IoMap *map){
	if(!map)
		return;
	for (size_t i = 0; i < 10; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map->addRead(i, "mgm", uid, gid, bytes);
			map->addWrite(i, "mgm", uid, gid, bytes * 3);
		}
	}
	for (size_t i = 10; i < 20; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map->addRead(i, "fdf", uid, gid, bytes);
			map->addWrite(i, "fdf", uid, gid, bytes * 4);
		}
	}
	for (size_t i = 20; i < 30; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		for (size_t it = 0, max = std::abs(rand() % 100); it < max;it++){
			size_t bytes = std::abs(rand() % 100000);
			map->addRead(i, "miniRT", uid, gid, bytes);
			map->addWrite(i, "miniRT", uid, gid, bytes * 9);
		}
	}
}

void fillGroup1(IoMap *map){
	map->addRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->addRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->addRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->addRead(1, "eos", 1, 1, std::abs(rand() % 100));
	map->addRead(1, "eos", 1, 1, std::abs(rand() % 100));
}

void fillGroup2(IoMap *map){
	map->addRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->addRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->addRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->addRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
	map->addRead(1, "jpeg", 2, 2, std::abs(rand() % 1000));
}

void fillGroup3(IoMap *map){
	map->addRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->addRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->addRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->addRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
	map->addRead(1, "jpeg", 3, 3, std::abs(rand() % 10000));
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

int testIoMapData(){
	std::multimap<std::string, std::optional<std::pair<double, double> > > data;
	std::vector<IoMap*> maps;
	size_t nbrOfMap = 3;

	for (size_t i = 0; i < nbrOfMap; i++)
		maps.push_back(new IoMap());
	for (size_t i = 0; i < nbrOfMap; i++)
		fillData(maps.at(i));

	for (size_t i = 0; i < 50; i++){
		std::lock_guard<std::mutex> lock(IoMap::_osMutex);

		for (auto &it : maps){
			data.insert({"mgm", it->getBandwidth("mgm", IoStat::Marks::READ, 2)});
			data.insert({"mgm", it->getBandwidth("mgm", IoStat::Marks::WRITE, 2)});
			data.insert({"uid_t: 2", it->getBandwidth<uid_t>(2, IoStat::Marks::READ)});
			data.insert({"uid_t: 2", it->getBandwidth<uid_t>(2, IoStat::Marks::WRITE)});
			data.insert({"gid_t: 1", it->getBandwidth<gid_t>(1, IoStat::Marks::READ)});
			data.insert({"gid_t: 1", it->getBandwidth<gid_t>(1, IoStat::Marks::WRITE)});
			if (io::IoMapDebug){
				for (auto &it : data){
					if (it.second.has_value()){
						std::cout << "map[" << it.first << "]: "
							<< "avrg: " << it.second->first << " | standard deviation: " << it.second->second << std::endl;
					}
					else
						std::cout << "no value" << std::endl;
				}
				std::cout << std::endl;
			}
		}
	}
	for (auto it = maps.begin(); it != maps.end(); it++){
		delete *it;
	}
	return 0;
}

/// Print std::pair
std::ostream& operator<<(std::ostream &os, const std::pair<double, double> &other){
	os << "[pair bandwidth] " << std::endl;
	os << C_BLUE << "{average: " << other.first <<
		", standard deviation: " << other.second <<  "}" << C_RESET << std::endl;
	return os;
}

/// Test if the average and standard deviation calcule are correct
int testIoMapSpecificCase(){
	IoMap map;

	map.addRead(1, "cernbox", 2, 1, 3534);
	map.addRead(1, "cernbox", 2, 1, 4562);
	map.addRead(1, "cernbox", 2, 1, 4573);
	map.addRead(1, "cernbox", 2, 1, 1332);
	map.addRead(1, "cernbox", 2, 1, 34563);
	map.addRead(1, "cernbox", 2, 1, 35);
	map.addRead(1, "cernbox", 2, 1, 544);

	std::optional<std::pair<double, double> > it = map.getBandwidth("cernbox", IoStat::Marks::READ);
	if (it.has_value()){
		if (static_cast<int>(it->first) != 7020 || static_cast<int>(it->second) != 12287)
			return -1;
	}
	else
		return -1;
	return 0;
}

void pairTmp(std::pair<double, double> *p1, std::pair<double, double> *p2, std::pair<double, double> *p3){
	p1->first = 42;
	p2->first = 65;
	p3->first = 56;
	double deviation = 0;
	deviation += std::pow(std::abs(50 - p1->first), 2);
	deviation += std::pow(std::abs(50 - p1->first), 2);
	deviation += std::pow(std::abs(26 - p1->first), 2);
	deviation = std::sqrt(deviation / 2);
	p1->second = deviation;
	deviation = 0;
	deviation += std::pow(std::abs(64 - p2->first), 2);
	deviation += std::pow(std::abs(97 - p2->first), 2);
	deviation += std::pow(std::abs(34 - p2->first), 2);
	deviation = std::sqrt(deviation / 2);
	p2->second = deviation;
	deviation = 0;
	deviation += std::pow(std::abs(97 - p3->first), 2);
	deviation += std::pow(std::abs(27 - p3->first), 2);
	deviation += std::pow(std::abs(44 - p3->first), 2);
	deviation = std::sqrt(deviation / 2);
	p3->second = deviation;
}

int testIoMapExactValue(){
	IoMap map;
	std::pair<double, double> p1;
	std::pair<double, double> p2;
	std::pair<double, double> p3;
	pairTmp(&p1, &p2, &p3);

	map.addWrite(1, "cernbox", 2, 1, 50);
	map.addWrite(1, "cernbox", 2, 1, 50);
	map.addWrite(1, "cernbox", 2, 1, 26);

	map.addWrite(1, "cernbox", 42, 42, 64);
	map.addWrite(1, "cernbox", 42, 42, 97);
	map.addWrite(1, "cernbox", 42, 42, 34);

	map.addWrite(1, "cernbox", 78, 5, 97);
	map.addWrite(1, "cernbox", 78, 5, 27);
	map.addWrite(1, "cernbox", 78, 5, 44);

	double avrg = 0;
	double deviation = 0;

	avrg += p1.first * 3;
	avrg += p2.first * 3;
	avrg += p3.first * 3;
	avrg /= 9;

	deviation += 2 * std::pow(p1.second, 2);
	deviation += 2 * std::pow(p2.second, 2);
	deviation += 2 * std::pow(p3.second, 2);
	deviation = std::sqrt(deviation / 6);

	std::optional<std::pair<double, double> > it = map.getBandwidth("cernbox", IoStat::Marks::WRITE);
	if (it.has_value()){
		if (it->first != avrg || it->second != deviation)
			return -1;
	}
	else
		return -1;
	return 0;
}

int testIoMapBigVolume(){
	std::vector<std::optional<std::pair<double, double> > > data;
	std::vector<IoMap*> maps;
	size_t nbrOfMap = 10000;

	IoMark begin;
	for (size_t i = 0; i < nbrOfMap; i++)
		maps.push_back(new IoMap());
	for (size_t i = 0; i < nbrOfMap; i++)
		fillData(maps.at(i));

	IoMark end;
	long diff = TIME_TO_CLEAN * 2 + 1;
	std::this_thread::sleep_for(std::chrono::seconds(diff));
	data.clear();
	for (auto &it : maps){
		for (size_t i = 0; i < 1000; i++){
			data.push_back(it->getBandwidth("mgm", IoStat::Marks::READ, 30));
			data.push_back(it->getBandwidth("mgm", IoStat::Marks::WRITE, 30));
			data.push_back(it->getBandwidth("fdf", IoStat::Marks::READ, 30));
			data.push_back(it->getBandwidth("fdf", IoStat::Marks::WRITE, 30));
			data.push_back(it->getBandwidth("miniRT", IoStat::Marks::READ, 30));
			data.push_back(it->getBandwidth("miniRT", IoStat::Marks::WRITE, 30));
		}
	}
	for (auto &it : data){
		if (it.has_value()){
			for (auto ite = maps.begin(); ite != maps.end(); ite++)
				delete *ite;
			return -1;
		}
	}

	for (auto it = maps.begin(); it != maps.end(); it++)
		delete *it;
	return 0;
}
