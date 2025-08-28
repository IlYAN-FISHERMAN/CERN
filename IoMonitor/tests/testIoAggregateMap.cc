//  File: testIoAggregateMap.cc
//  Author: Ilkay Yanar - 42Lausanne / CERN
//  ----------------------------------------------------------------------

/*************************************************************************
 *  EOS - the CERN Disk Storage System                                   *
 *  Copyright (C) 2025 CERN/Switzerland                                  *
 *                                                                       *
 *  This program is free software: you can redistribute it and/or modify *
 *  it under the terms of the GNU General Public License as published by *
 *  the Free Software Foundation, either version 3 of the License, or    *
 *  (at your option) any later version.                                  *
 *                                                                       *
 *  This program is distributed in the hope that it will be useful,      *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *  GNU General Public License for more details.                         *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 *************************************************************************/

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
static int printSummary(IoAggregateMap &map, size_t winTime, const T index){
	std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: "
		<< winTime << C_GREEN << "][" << C_CYAN << "summary of appName: " << std::string(index)
		<< C_GREEN << "]" << C_RESET << std::endl;
	std::cout << C_CYAN << map.getSummary(winTime, index) << C_RESET << std::endl;
	return 0;
}
template <typename T>
static int printSummary(IoAggregateMap &map, size_t winTime, io::TYPE type, const T index){
	if (type == io::TYPE::UID)
		std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: "
			<< winTime << C_GREEN << "][" << C_CYAN << "summary of uid: " << index
			<< C_GREEN << "]" << C_RESET << std::endl;
	else if (type == io::TYPE::GID)
		std::cout << C_GREEN << "[" << C_CYAN << "Summary winTime: "
			<< winTime << C_GREEN << "][" << C_CYAN << "summary of gid: " << index
			<< C_GREEN << "]" << C_RESET << std::endl;
	else{
		std::cout << "printSummay failed" << std::endl;
		return -1;
	}
	std::cout << C_CYAN <<  map.getSummary(winTime, type, index) << C_RESET << std::endl;
	return 0;
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
		map.addRead(1, "eos", 12, 11, std::abs(rand())%10000);
		map.addRead(1, "eos", 1, 11, std::abs(rand())%10000);
		map.addRead(1, "mgm", 1, 11, std::abs(rand())%10000);
		map.addRead(1, "fdf", 12, 1, std::abs(rand())%10000);
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
	if (eos->rSize < 40 || eos->wSize < 40
		|| mgm->rSize < 20 || mgm->wSize < 20
		|| fdf->rSize < 20 || fdf->wSize < 20
		|| uid->rSize < 40 || uid->wSize < 40
		|| gid->rSize < 60 || gid->wSize < 60)
		return -1;
	return 0;
}

int testIoAggregateMapCopy(){
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
		map.addRead(1, "eos", 12, 11, std::abs(rand())%10000);
		map.addRead(1, "eos", 1, 11, std::abs(rand())%10000);
		map.addRead(1, "mgm", 1, 11, std::abs(rand())%10000);
		map.addRead(1, "fdf", 12, 1, std::abs(rand())%10000);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	{
		// auto eos = map.getSummary(3600, "eos");
		// auto mgm = map.getSummary(3600, "mgm");
		// auto fdf = map.getSummary(3600, "fdf");
		// auto uid = map.getSummary(3600, io::TYPE::UID, 12);
		// auto gid = map.getSummary(3600, io::TYPE::GID, 11);
		//
		// if (!eos.has_value() || !mgm.has_value() || !fdf.has_value() ||
		// 	!uid.has_value() || !gid.has_value())
		// 	return -1;
		// if (eos->rSize < 40 || eos->wSize < 40
		// 	|| mgm->rSize < 20 || mgm->wSize < 20
		// 	|| fdf->rSize < 20 || fdf->wSize < 20
		// 	|| uid->rSize < 40 || uid->wSize < 40
		// 	|| gid->rSize < 60 || gid->wSize < 60)
		// 	return -1;
	}
	{
		IoAggregateMap copyMap = map;
		auto eos = copyMap.getSummary(3600, "eos");
		auto mgm = copyMap.getSummary(3600, "mgm");
		auto fdf = copyMap.getSummary(3600, "fdf");
		auto uid = copyMap.getSummary(3600, io::TYPE::UID, 12);
		auto gid = copyMap.getSummary(3600, io::TYPE::GID, 11);

		if (!eos.has_value() || !mgm.has_value() || !fdf.has_value() ||
			!uid.has_value() || !gid.has_value())
			return -1;
		if (eos->rSize < 40 || eos->wSize < 40
			|| mgm->rSize < 20 || mgm->wSize < 20
			|| fdf->rSize < 20 || fdf->wSize < 20
			|| uid->rSize < 40 || uid->wSize < 40
			|| gid->rSize < 60 || gid->wSize < 60)
			return -1;
	}
	{
		IoAggregateMap copyMap(map);
		auto eos = copyMap.getSummary(3600, "eos");
		auto mgm = copyMap.getSummary(3600, "mgm");
		auto fdf = copyMap.getSummary(3600, "fdf");
		auto uid = copyMap.getSummary(3600, io::TYPE::UID, 12);
		auto gid = copyMap.getSummary(3600, io::TYPE::GID, 11);

		if (!eos.has_value() || !mgm.has_value() || !fdf.has_value() ||
			!uid.has_value() || !gid.has_value())
			return -1;
		if (eos->rSize < 40 || eos->wSize < 40
			|| mgm->rSize < 20 || mgm->wSize < 20
			|| fdf->rSize < 20 || fdf->wSize < 20
			|| uid->rSize < 40 || uid->wSize < 40
			|| gid->rSize < 60 || gid->wSize < 60)
			return -1;
	}

	return 0;
}

static void printUsage(){
	std::cout << "Usage:" << std::endl;
	std::cout << "./monitor [command] [options...]" << std::endl;
	std::cout << std::endl;

	std::cout << "META OPTIONS" << std::endl;
	std::cout << "  h, help \tshow list of command-line options." << std::endl;
	std::cout << std::endl;

	std::cout << "COMMANDS" << std::endl;
	std::cout << "  add [window], \t\t\t\tadd a window to the map" << std::endl;
	std::cout << "  set [window][track][...], \t\t\tset track to a window, multiple track can be set" << std::endl;
	std::cout << "  r [fileId][appName][uid][gid][bytes], \tadd a read input to the map" << std::endl;
	std::cout << "  w [fileId][appName][uid][gid][bytes], \tadd a write input to the map" << std::endl;
	std::cout << "  m, \t\t\t\t\t\tprint the IoAggregate map" << std::endl;
	std::cout << "  p [window][track], \t\t\t\tprint the summary of a track" << std::endl;
	std::cout << "  fill, \t\t\t\t\tfill the map with I/O" << std::endl;
	std::cout << "  c, \t\t\t\t\t\tclear the terminal" << std::endl;
	std::cout << "  exit, \t\t\t\t\texit monitor" << std::endl;
	std::cout << std::endl;

	std::cout << "OPTIONS" << std::endl;
	std::cout << "  window, \tsize_t number" << std::endl;
	std::cout << "  track, \ttrack can be a appName/uid/gid, if it's a uid/gid you have to specify it" << std::endl;
	std::cout << "  fileId, \tsize_t number" << std::endl;
	std::cout << "  appName, \tstring" << std::endl;
	std::cout << "  uid, \t\tuid_t number" << std::endl;
	std::cout << "  gid, \t\tgid_t number" << std::endl;
	std::cout << "  bytes, \tsize_t number" << std::endl;
	std::cout << std::endl;

	std::cout << "EXEMPLE" << std::endl;
	std::cout << "  [uid set],\t\t$ set 60 uid 14" << std::endl;
	std::cout << "  [appName set],\t$ set 60 eos" << std::endl;
	std::cout << "  [gid set],\t\t$ set 60 gid 12" << std::endl;
	std::cout << "  [multiple set],\t$ set 60 eos uid 12 gid 42 mgm fst" << std::endl;
	std::cout << std::endl;
	std::cout << "  [add window],\t\t$ add 60" << std::endl;
	std::cout << "  [add window],\t\t$ add 3600" << std::endl;
	std::cout << std::endl;
	std::cout << "  [add read],\t\t$ r 10 eos 250 13 241351" << std::endl;
	std::cout << "  [add write],\t\t$ w 13 eos 43 7 581" << std::endl;
	std::cout << std::endl;
	std::cout << "  [print summary],\t$ p 3600 uid 14" << std::endl;
	std::cout << "  [print summary],\t$ p 3600 eos" << std::endl;
	std::cout << std::endl;
}

int setTrack(IoAggregateMap &map, std::stringstream &stream){
	int code = 0;
	size_t winTime = 0;
	size_t uid = 0;
	size_t gid = 0;
	std::string cmd;

	if (stream >> winTime){
		while (stream >> cmd){
			if (cmd == "uid"){
				if (stream >> uid)
					code = map.setTrack(winTime, io::TYPE::UID, uid);
				else{
					std::cerr << "Monitor: Error: bad uid number" << std::endl;
					return -1;
				}
			}
			else if (cmd == "gid"){
				if (stream >> gid)
					code = map.setTrack(winTime, io::TYPE::GID, gid);
				else{
					std::cerr << "Monitor: Error: bad gid number" << std::endl;
					return -1;
				}
			}
			else
				code = map.setTrack(winTime, cmd);
			if (code != 0)
				return code;
		}
		if (cmd.empty())
			return -1;
	}
	else
		return -1;
	return 0;
}

int addWindow(IoAggregateMap &map, std::stringstream &stream){
	char *tmp = NULL;
	long winTime = 0;
	std::string cmd;

	while (stream >> cmd){
		winTime = std::strtol(cmd.c_str(), &tmp, 10);
		if (!*tmp){
			if (winTime < 0 || map.addWindow(winTime))
				return -1;
		}
		else
			return -1;
	}

	return 0;
}

int printSums(IoAggregateMap &map, std::stringstream &stream){
	size_t winTime = 0;
	std::string cmd;
	int code = 0;
	size_t uid = 0;
	size_t gid = 0;

	while (true){
		if (stream >> winTime >> cmd){
			if (cmd == "uid" && stream >> uid)
				code = printSummary(map, winTime, io::TYPE::UID, uid);
			else if (cmd == "gid" && stream >> gid)
				code = printSummary(map, winTime, io::TYPE::GID, gid);
			else
				code = printSummary(map, winTime, cmd);
			if (code)
				return -1;
		}
		else if (!stream.eof())
			return -1;
		else
			break;
	}

	return 0;
}

int testIoAggregateMapInteract(){
	IoAggregateMap map;

	while(true){
		std::string input;
		std::cout << "[IoMonitor]-> ";
		std::getline(std::cin, input);
		if (input == "m")
			std::cout << map << std::endl;
		else if (input == "c")
			std::cout << "\033c";
		else if (input == "exit"){
			std::cout << "exit" << std::endl;
			break ;
		}
		else{
			std::string cmd;
			std::stringstream stream(input);
			if (stream >> cmd){
				int winTime = 0;
				uid_t uid = 0;
				gid_t gid = 0;
				size_t bytes = 0;
				if (cmd == "set"){
					if (!setTrack(map, stream))
						std::cout << "track successfully set" << std::endl;
					else
						std::cout << "track set failed" << std::endl;
				}
				else if (cmd == "add"){
					if (!addWindow(map, stream))
						std::cout << "window successfully set" << std::endl;
					else
						std::cout << "window set failed" << std::endl;
				}
				else if (cmd == "r"){
					int fileId = 0;
					std::string appName;
					if (stream >> fileId >> appName >> uid >> gid >> bytes){
						map.addRead(fileId, appName, uid, gid, bytes);
						std::cout << "add read succeed" << std::endl;
					}
					else
						std::cout << "add read failed" << std::endl;
				}
				else if (cmd == "w"){
					int fileId = 0;
					std::string appName;
					if (stream >> fileId >> appName >> uid >> gid >> bytes){
						map.addWrite(fileId, appName, uid, gid, bytes);
						std::cout << "add write succeed" << std::endl;
					}
					else
						std::cout << "add write failed" << std::endl;
				}
				else if (cmd == "p"){
					if (!printSums(map, stream))
						std::cout << "print Summary succeed" << std::endl;
					else
						std::cout << "print Summary failed" << std::endl;
				}
				else if (cmd == "s"){
					long index = 0;
					if (stream >> winTime){
						if (stream >> index){
							index = map.shiftWindow(winTime, index);
							if (index == -1)
								std::cout << "shift window " << winTime << " failed" << std::endl;
							else
								std::cout << "shift window " << winTime << " at " << index << std::endl;
						}
						else{
							index = map.shiftWindow(winTime);
							if (index == -1)
								std::cout << "shift window " << winTime << " failed" << std::endl;
							else
								std::cout << "shift window " << winTime << " at " << index << std::endl;
						}
					}
				}
				else if (cmd == "fill"){
					if (stream >> cmd)
						std::cout << "Monitor: command not found: " << input << std::endl;
					else{
						if (!fillDataInteract(map))
							std::cout << "fill map succeed" << std::endl;
						else
							std::cout << "fill map failed" << std::endl;
					}
				}
				else if (cmd == "h" || cmd == "help")
					printUsage();
				else
					std::cout << "Monitor: command not found: " << input << std::endl;
			}
		}
	}
	return 0;
}
