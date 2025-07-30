#include "tester.hh"

void fillData(IoMap *map){
	for (size_t i = 0, j = std::abs(rand() % 10); i < j; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		size_t bytes = std::abs(rand() % 100000);
		map->AddRead(1, "mgm", uid, gid, bytes);
	}
	for (size_t i = 0, j = std::abs(rand() % 10); i < j; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		size_t bytes = std::abs(rand() % 100000);
		map->AddRead(1, "pdf", uid, gid, bytes);
	}
	for (size_t i = 0, j = std::abs(rand() % 10); i < j; i++){
		int uid = std::abs(rand() % 100);
		int gid = std::abs(rand() % 100);
		size_t bytes = std::abs(rand() % 100000);
		map->AddRead(1, "eos", uid, gid, bytes);
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

int testIoMap(){
	IoMap map;

	fillData(&map);
	std::cout << map;
	// std::this_thread::sleep_for(std::chrono::seconds(2));
	// std::vector<std::optional<std::pair<double, double> > > data;
	//
	// fillData(&map);
	// data.push_back(map.getBandwidth("mgm", IoStat::Marks::READ, 3));
	// data.push_back(map.getBandwidth("pdf", IoStat::Marks::READ, 3));
	// data.push_back(map.getBandwidth<uid_t>(10, IoStat::Marks::READ, 3));
	// data.push_back(map.getBandwidth<gid_t>(2, IoStat::Marks::READ, 3));
	// for (auto it : data){
	// 	if (it.has_value())
	// 		std::cout << "avrg: " << it->first << " | standard deviation: " << it->second << std::endl;
	// 	else
	// 		std::cout << "no value" << std::endl;
	// 	// std::this_thread::sleep_for(std::chrono::seconds(5));
	// }
	return 0;
}

int testInteractiveIoMap(){
	IoMap *map = new IoMap();

	while(true){
		std::lock_guard<std::mutex> lock(IoMap::_osMutex);
		std::cout << "ran: " << rand() << std::endl;
		std::string input;
		std::cout << "[IoMap]-> ";
		std::getline(std::cin, input);
		if (input.empty())
			continue;
		std::stringstream os(input);
		std::string cmd;
		os >> cmd;
		if (cmd == "exit"){
			std::cout << "exit" << std::endl;
			break;
		}
		else if (cmd == "print" || cmd == "p"){
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
		else if (cmd == "fill"){
			std::cout << "fill data begin!" << std::endl;

			cmd.clear();
			os >> cmd;
			if (cmd.empty()){
				fillData(map);
				std::cout << "full fill" << std::endl;
			}
			else if (cmd == "-g" && os >> cmd){
				if (cmd == "1")
					fillGroup1(map);
				else if (cmd == "2")
					fillGroup2(map);
				else if (cmd == "3")
					fillGroup3(map);
				else
					std::cerr << "IoMap: " << cmd << " :group not found" << std::endl;
			}
			else
				std::cerr << "IoMap: " << input << " : Need group id [1, 2, 3]" << std::endl;
			std::cout << "fill data end" << std::endl;
		}
		else if (cmd == "clear" || cmd == "c")
			std::cout << "\033c";
		else
			std::cerr << "IoMap: " << input << " :command not found" << std::endl;
		std::cout << std::endl;
	}
	delete map;
	return 0;
}
