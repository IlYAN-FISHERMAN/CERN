#include "tester.hh"

std::ostream& operator<<(std::ostream &os, const std::optional<IoStatSummary> &opt){
	if (!opt.has_value()){
		os << "empty" << std::endl;
		return os;
	}
	IoStatSummary other = opt.value();
	os << "[READ]: ";
	if (other.readBandwidth.has_value())
		os << "average: " << other.readBandwidth->first
			<< " standard deviation: " << other.readBandwidth->second
			<< " size: " << other.rSize << std::endl;
	else
		os << "empty" << std::endl;
	os << "[WRITE]: ";
	if (other.writeBandwidth.has_value())
		os << "average: " << other.writeBandwidth->first
			<< " standard deviation: " << other.writeBandwidth->second
			<< " size: " << other.wSize << std::endl;
	else
		os << "empty" << std::endl;
	return os;
}


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
	std::vector<std::unique_ptr<IoMap> > maps;
	size_t nbrOfMap = 10;

	for (size_t i = 0; i < nbrOfMap; i++)
		maps.push_back(std::make_unique<IoMap>());
	for (size_t i = 0; i < nbrOfMap; i++)
		fillData(maps.at(i).get());

	for (size_t i = 0; i < 50; i++){
		std::lock_guard<std::mutex> lock(IoMap::_osMutex);

		for (auto &it : maps){
			data.insert({"mgm", it->getBandwidth("mgm", IoStat::Marks::READ, 2)});
			data.insert({"mgm", it->getBandwidth("mgm", IoStat::Marks::WRITE, 2)});
			data.insert({"uid_t: 2", it->getBandwidth(io::TYPE::UID, 2, IoStat::Marks::READ)});
			data.insert({"uid_t: 2", it->getBandwidth(io::TYPE::UID, 2, IoStat::Marks::WRITE)});
			data.insert({"gid_t: 1", it->getBandwidth(io::TYPE::GID, 1, IoStat::Marks::READ)});
			data.insert({"gid_t: 1", it->getBandwidth(io::TYPE::GID, 1, IoStat::Marks::WRITE)});
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

	map.addRead(1, "cernbox", 2, 1, 3531);
	map.addRead(1, "cernbox", 2, 1, 4562);
	map.addRead(1, "cernbox", 2, 1, 4573);
	map.addRead(1, "cernbox", 2, 1, 1332);
	map.addRead(1, "cernbox", 2, 1, 34563);
	map.addRead(1, "cernbox", 2, 1, 35);
	map.addRead(1, "cernbox", 2, 1, 544);

	std::optional<std::pair<double, double> > it = map.getBandwidth("cernbox", IoStat::Marks::READ);
	if (it.has_value()){
		if (static_cast<int>(it->first) != 7020 || static_cast<int>(it->second) != 11376)
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
	deviation = std::sqrt(deviation / 3);
	p1->second = deviation;
	deviation = 0;
	deviation += std::pow(std::abs(64 - p2->first), 2);
	deviation += std::pow(std::abs(97 - p2->first), 2);
	deviation += std::pow(std::abs(34 - p2->first), 2);
	deviation = std::sqrt(deviation / 3);
	p2->second = deviation;
	deviation = 0;
	deviation += std::pow(std::abs(97 - p3->first), 2);
	deviation += std::pow(std::abs(27 - p3->first), 2);
	deviation += std::pow(std::abs(44 - p3->first), 2);
	deviation = std::sqrt(deviation / 3);
	p3->second = deviation;
}


int testIoMapExactValue(){
	IoMap map;

	// Calculate raw data
	std::pair<double, double> p1;
	std::pair<double, double> p2;
	std::pair<double, double> p3;

	pairTmp(&p1, &p2, &p3);
	double avrg = 0;
	double deviation = 0;

	// weighted average
	avrg += p1.first * 3;
	avrg += p2.first * 3;
	avrg += p3.first * 3;
	avrg /= 9;

	// weighted standard deviation
	deviation += 3 * (std::pow(p1.second, 2) + std::pow(p1.first - avrg, 2));
	deviation += 3 * (std::pow(p2.second, 2) + std::pow(p2.first - avrg, 2));
	deviation += 3 * (std::pow(p3.second, 2) + std::pow(p3.first - avrg, 2));
	deviation = std::sqrt(deviation / 9);

	// Test IoMap function
	map.addWrite(1, "cernbox", 2, 1, 50);
	map.addWrite(1, "cernbox", 2, 1, 50);
	map.addWrite(1, "cernbox", 2, 1, 26);

	map.addWrite(1, "cernbox", 42, 42, 64);
	map.addWrite(1, "cernbox", 42, 42, 97);
	map.addWrite(1, "cernbox", 42, 42, 34);

	map.addWrite(1, "cernbox", 78, 5, 97);
	map.addWrite(1, "cernbox", 78, 5, 27);
	map.addWrite(1, "cernbox", 78, 5, 44);

	// if no data or difference between raw data and function return -1
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
	std::vector<std::unique_ptr<IoMap> > maps;
	size_t nbrOfMap = 100;

	IoMark begin;
	for (size_t i = 0; i < nbrOfMap; i++)
		maps.push_back(std::make_unique<IoMap>());
	for (size_t i = 0; i < nbrOfMap; i++)
		fillData(maps.at(i).get());

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
		if (it.has_value())
			return -1;
	}
	return 0;
}

int testIoMapSummary(){
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

	map.addRead(1, "cernbox", 2, 1, 50);
	map.addRead(1, "cernbox", 2, 1, 50);
	map.addRead(1, "cernbox", 2, 1, 26);

	map.addRead(1, "cernbox", 42, 42, 64);
	map.addRead(1, "cernbox", 42, 42, 97);
	map.addRead(1, "cernbox", 42, 42, 34);

	map.addRead(1, "cernbox", 78, 5, 97);
	map.addRead(1, "cernbox", 78, 5, 27);
	map.addRead(1, "cernbox", 78, 5, 44);


	double rAvrg = 0;
	double rDeviation = 0;

	double wAvrg = 0;
	double wDeviation = 0;

	rAvrg += p1.first * 3;
	rAvrg += p2.first * 3;
	rAvrg += p3.first * 3;
	rAvrg /= 9;

	rDeviation += 2 * std::pow(p1.second, 2);
	rDeviation += 2 * std::pow(p2.second, 2);
	rDeviation += 2 * std::pow(p3.second, 2);
	rDeviation = std::sqrt(rDeviation / 6);

	wAvrg += p1.first * 3;
	wAvrg += p2.first * 3;
	wAvrg += p3.first * 3;
	wAvrg /= 9;

	wDeviation += 2 * std::pow(p1.second, 2);
	wDeviation += 2 * std::pow(p2.second, 2);
	wDeviation += 2 * std::pow(p3.second, 2);
	wDeviation = std::sqrt(wDeviation / 6);

	// std::cout << map.getSummary("cernbox") << std::endl;
	std::optional<std::pair<double, double> > read = map.getBandwidth("cernbox", IoStat::Marks::READ);
	std::optional<std::pair<double, double> > write = map.getBandwidth("cernbox", IoStat::Marks::READ);
	if (read.has_value() && write.has_value()){
		if (read->first != rAvrg || read->second != rDeviation
			|| write->first != wAvrg || write->second != wDeviation)
			return -1;
	}
	else
		return -1;

	// std::optional<IoStatSummary> summary = map.getSummary("cernbox");
	// std::cout << summary << std::endl
	// if (summary.has_value()){
	// 	if (!summary->readBandwidth.has_value() || !summary->writeBandwidth.has_value())
	// 		return -1;
	// 	if (summary->readBandwidth->first != rAvrg || summary->readBandwidth->second != rDeviation
	// 		|| summary->writeBandwidth->first != wAvrg || summary->writeBandwidth->second != wDeviation)
	// 		return -1;
	// 	else if (summary->readBandwidth->first != read->first || summary->readBandwidth->second != read->second
	// 		|| summary->writeBandwidth->first != write->first || summary->writeBandwidth->second != write->second)
	// 		return -1;
	// }
	return 0;
}

int testIoMapIds(){
	IoMap map;

	map.addRead(1, "eos", 40, 24, 564);
	map.addRead(1, "eos", 40, 24, 443);
	map.addRead(1, "eos", 40, 24, 554);
	map.addRead(1, "eos", 40, 24, 20);
	map.addRead(1, "eos", 40, 24, 4220);
	map.addRead(1, "eos", 40, 24, 24250);
	map.addRead(1, "eos", 42, 24, 125);
	map.addRead(1, "eos", 42, 24, 24);
	map.addRead(1, "eos", 42, 24, 24);
	map.addRead(1, "eos", 42, 24, 24);
	map.addRead(1, "eos", 42, 24, 48);
	map.addRead(4, "eos", 56, 44, 15);
	map.addRead(4, "eos", 56, 44, 142);
	map.addRead(4, "eos", 56, 44, 155);

	{
		auto it = map.getBandwidth("eos", IoStat::Marks::READ);
		if (it.has_value())
			std::cout << it.value() << std::endl;
		else
			std::cout << "empty\n\n";
	}
	// {
	// 	auto it = map.getBandwidth(io::TYPE::GID, 24, IoStat::Marks::READ);
	// 	if (it.has_value())
	// 		std::cout << it.value() << std::endl;
	// 	else
	// 		std::cout << "empty\n\n";
	// }
	// {
	// 	auto it = map.getBandwidth(io::TYPE::UID, 24, IoStat::Marks::READ);
	// 	if (it.has_value())
	// 		std::cout << it.value() << std::endl;
	// 	else
	// 		std::cout << "empty\n\n";
	// }

	// std::cout << map << std::endl;
	return 0;
}
