#include "tester.hh"

void fillData(IoMap *map){
	map->AddRead(42, "mgm", 4, 2, 1);
	map->AddRead(42, "mgm", 4, 2, 532);
	map->AddRead(42, "mgm", 4, 2, 241);
	map->AddRead(42, "mgm", 4, 2, 421);
	map->AddRead(42, "mgm", 4, 2, 721);

	map->AddRead(42, "mgm", 4, 4, 9042);
	map->AddRead(42, "mgm", 4, 4, 9042);
	map->AddRead(42, "mgm", 4, 4, 4290);
	map->AddRead(42, "mgm", 4, 4, 5450);
	map->AddRead(42, "mgm", 4, 4, 540);

	map->AddRead(42, "mgm", 4, 44, 852);
	map->AddRead(42, "mgm", 4, 44, 425);
	map->AddRead(42, "mgm", 4, 44, 312);

	map->AddRead(42, "mgm", 4752, 2342, 70);
	map->AddRead(42, "mgm", 4752, 2342, 7244);

	map->AddRead(42, "pdf", 4, 2, 6340);
	map->AddRead(42, "pdf", 4, 2, 5);

	map->AddRead(42, "pdf", 10, 2, 100);
	map->AddRead(42, "pdf", 10, 2, 200);
	map->AddRead(42, "pdf", 10, 2, 654);
	map->AddRead(42, "pdf", 10, 2, 245);
	map->AddRead(42, "pdf", 10, 2, 182);

	map->AddRead(42, "pdf", 10, 500, 400);
	map->AddRead(42, "pdf", 10, 500, 500);

	map->AddRead(42, "pdf", 4, 1, 6340);

	map->AddRead(24, "pdf", 10, 2, 100);
	map->AddRead(24, "pdf", 10, 2, 200);
	map->AddRead(24, "pdf", 10, 2, 654);
	map->AddRead(24, "pdf", 10, 2, 245);
	map->AddRead(24, "pdf", 10, 2, 182);

	map->AddRead(50, "pdf", 10, 2, 424);
	map->AddRead(50, "pdf", 10, 2, 28355);
	map->AddRead(50, "pdf", 52, 2, 325);
	map->AddRead(50, "pdf", 52, 2, 985);
	map->AddRead(50, "pdf", 52, 2, 4562);
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
	IoMap *map = new IoMap();

	while(true){
		std::lock_guard<std::mutex> lock(IoMap::_osMutex);
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
	// std::this_thread::sleep_for(std::chrono::seconds(2));
	// std::vector<std::optional<std::pair<double, double> > > data;
	//
	// data->push_back(map->getBandwidth("mgm", IoStat::Marks::READ, 3));
	// data->push_back(map->getBandwidth("pdf", IoStat::Marks::READ, 3));
	// data->push_back(map->getBandwidth<uid_t>(10, IoStat::Marks::READ, 3));
	// data->push_back(map->getBandwidth<gid_t>(2, IoStat::Marks::READ, 3));
	// for (auto it : data){
	// 	if (it->has_value())
	// 		std::cout << "avrg: " << it->first << " | standard deviation: " << it->second << std::endl;
	// 	else
	// 		std::cout << "no value" << std::endl;
	// 	// std::this_thread::sleep_for(std::chrono::seconds(5));
	// }
	return 0;
}
