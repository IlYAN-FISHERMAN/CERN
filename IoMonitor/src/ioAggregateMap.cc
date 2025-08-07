//  File: ioAggregateMap.cc
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

#include "../include/ioAggregateMap.hh"

#define TIME_TO_UPDATE 60


IoAggregateMap::IoAggregateMap() : _running(true){
	_thread = std::thread(&IoAggregateMap::updateAggregateLoop, this);
}

IoAggregateMap::IoAggregateMap(int) : _running(true){
	// _thread = std::thread(&IoAggregateMap::aggregationLoop, this);
}

IoAggregateMap::~IoAggregateMap(){}

std::vector<size_t> IoAggregateMap::getAvailableWindows() const{
	return (std::vector<size_t>(_aggregates.begin()->first, _aggregates.end()->first));
}

void IoAggregateMap::addRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes){
	_map.addRead(inode, app, uid, gid, rbytes);
}

void IoAggregateMap::addWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes){
	_map.addWrite(inode, app, uid, gid, wbytes);
}

void IoAggregateMap::updateAggregateLoop(){
	while (true){
		std::this_thread::sleep_for(std::chrono::seconds(TIME_TO_UPDATE));
		if (!_running)
			break;
		// Update IoAggregates
	}
}




const IoMap& IoAggregateMap::getIoMap() const{return _map;}

std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator IoAggregateMap::begin(){return _map.begin();}
std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator IoAggregateMap::end(){return _map.end();}

std::ostream& operator<<(std::ostream &os, const IoAggregateMap &other){
	os << other.getIoMap() << std::endl << std::endl;
	return os;
}
