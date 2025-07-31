//  File: testIoStat.cc
//  Author: Ilkay Yanar - 42Lausanne /CERN
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

void fillIoStat(IoStat &io, int nbr = 10, int range = 100000, double seconds = 0.1){
	if (config::IoStatDebug)
		IoStat::printInfo(std::cout, "fill IoStat");
	for (int i = 0; i < nbr; i++){
		io.addWrite(rand() % range);
		io.addRead(rand() % range * 0.5);
		if (seconds > 0)
			usleep(seconds);
	}
	if (config::IoStatDebug)
		IoStat::printInfo(std::cout, "fill end");
}

int getBandWidth(IoStat &io, IoStat::Marks enumMark, size_t seconds = 10){
	if (config::IoStatDebug){
		std::cout << std::endl;
		IoStat::printInfo(std::cout, "Get bandwidth from the last " + std::to_string(seconds) + "s");
	}

	if (enumMark != IoStat::Marks::READ && enumMark != IoStat::Marks::WRITE)
		return -1;
	
	size_t size = 0;
	std::pair<double, double> bandWidth = io.bandWidth(enumMark, &size, seconds);

	if (config::IoStatDebug){
		std::cout << std::fixed;
		if (enumMark == IoStat::Marks::READ)
			std::cout << "\t[Read:" << size << "/" << io.getSize(enumMark) << "]: average: "
				<< bandWidth.first << " | standard deviation: " << bandWidth.second << std::endl;
		else
			std::cout << "\t[Write:" << size << "/" << io.getSize(enumMark) << "]: average: "
				<< bandWidth.first << " | standard deviation: " << bandWidth.second << std::endl;
		std::cout << std::endl;
	}
	return 0;
}

int cleanMarks(IoStat &io, IoStat::Marks enumMark, int seconds){
	if (config::IoStatDebug)
		IoStat::printInfo(std::cout, "Clean everything after " + std::to_string(seconds) + "s");
	int code = io.cleanOldsMarks(enumMark, seconds);
	if (config::IoStatDebug)
		std::cout << std::endl;
	return code;
}

int testIoStatFillData(){
	IoStat io(4, "mgm", 2, 2);

	fillIoStat(io, 1000000, 100, 0);
	if (getBandWidth(io, IoStat::Marks::READ, 1) < 0
		|| getBandWidth(io, IoStat::Marks::WRITE, 1) < 0)
			return -1;

	if (config::IoStatDebug)
		IoStat::printInfo(std::cout, " [ Error tests ]");
	getBandWidth(io, IoStat::Marks::WRITE, 100);
	size_t size = io.cleanOldsMarks(IoStat::Marks::WRITE, 0);
	if (config::IoStatDebug)
		IoStat::printInfo(std::cout, "Erased " + std::to_string(size) + " element");
	io.addWrite(0);
	io.addWrite(0);
	io.addWrite(0);
	getBandWidth(io, IoStat::Marks::WRITE, 1000);
	getBandWidth(io, IoStat::Marks::WRITE, 0);
	getBandWidth(io, IoStat::Marks::READ, 10);
	io.cleanOldsMarks(IoStat::Marks::READ, 1000);
	getBandWidth(io, IoStat::Marks::READ, 10);
	getBandWidth(io, IoStat::Marks::READ, 0);


	size = io.cleanOldsMarks(IoStat::Marks::WRITE, 0);
	size = io.cleanOldsMarks(IoStat::Marks::READ, 0);
	fillIoStat(io, 1000000, 100, 0);
	for (size_t i = 0; i < 10; i++){
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		getBandWidth(io, IoStat::Marks::WRITE, 1);
		getBandWidth(io, IoStat::Marks::READ, 1);
	}
	return 0;
}

int testIoStatCleaning(){
	IoStat io(4, "qukdb", 2, 2);

	fillIoStat(io, 1000000, 100, 0);
	if (cleanMarks(io, IoStat::Marks::READ, 1) < 0)
		return -1;
	return 0;
}
