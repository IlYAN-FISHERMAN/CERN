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

void sleeping(double seconds){
	if (DEBUG){
		std::string time = "sleeping " + std::to_string(seconds) + std::string("s...");
		IoStat::printInfo(std::cout, time);
	}
	sleep(seconds);
}

void fillIoStat(IoStat &io, int nbr = 10, int range = 100000, double seconds = 0.1){
	if (DEBUG)
		IoStat::printInfo(std::cout, "fill IoStat");
	for (int i = 0; i < nbr; i++){
		io.addWrite(rand() % range);
		io.addRead(rand() % range);
		sleeping(seconds);
	}
}

int getBandWidth(IoStat &io, IoStat::Marks enumMark, int seconds = 10){
	if (DEBUG){
		std::cout << std::endl;
		IoStat::printInfo(std::cout, "Get bandwidth from the last " + std::to_string(seconds) + "s");
	}

	if (enumMark != IoStat::Marks::READ && enumMark != IoStat::Marks::WRITE)
		return -1;

	std::pair<double, double> bandWidth = io.bandWidth(enumMark, NULL, seconds);
	std::bit_and<double>();
	if (bandWidth.first == 0 && bandWidth.second == 0 && io.getSize(enumMark) > 0)
		return -1;

	if (DEBUG){
		if (enumMark == IoStat::Marks::READ)
			std::cout << "\t[Read]: average: " << bandWidth.first << " | standard deviation: " << bandWidth.second << std::endl;
		else
			std::cout << "\t[Write]: average: " << bandWidth.first << " | standard deviation: " << bandWidth.second << std::endl;
		std::cout << std::endl;
	}
	return 0;
}

int cleanMarks(IoStat &io, IoStat::Marks enumMark, int seconds){
	if (DEBUG)
		IoStat::printInfo(std::cout, "Clean everything after " + std::to_string(seconds) + "s");
	int code = io.cleanOldsMarks(enumMark, seconds);
	if (DEBUG)
		std::cout << std::endl;
	return code;
}

int testIoStatFillData(){
	IoStat io(4, "mgm", 2, 2);

	fillIoStat(io, 100000, 10000000);
	sleeping(10);
	if (getBandWidth(io, IoStat::Marks::READ, 2) < 0 ||
		getBandWidth(io, IoStat::Marks::READ, 2) < 0 ||
		getBandWidth(io, IoStat::Marks::READ, 2) < 0)
			return -1;
	return 0;
}

int testIoStatCleaning(){
	IoStat io(4, "qukdb", 2, 2);

	fillIoStat(io, 10000, 10000000);
	fillIoStat(io, 10000, 10000000);
	if (cleanMarks(io, IoStat::Marks::READ, 1) < 0 ||
		cleanMarks(io, IoStat::Marks::WRITE, 1) < 0)
		return -1;
	return 0;
}
