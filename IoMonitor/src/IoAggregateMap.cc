//  File: IoAggregateMap.cc
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

#include "../include/IoAggregateMap.hh"

IoAggregateMap::IoAggregateMap() : _running(true){
	_thread = std::thread(&IoAggregateMap::updateAggregateLoop, this);
}

IoAggregateMap::IoAggregateMap(int) : _running(true){
	// _thread = std::thread(&IoAggregateMap::aggregationLoop, this);
}

IoAggregateMap::~IoAggregateMap(){
	std::lock_guard<std::mutex> lock(_mutex);
	_running = false;
}

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
	auto next_tick = std::chrono::steady_clock::now() + std::chrono::seconds(TIME_TO_UPDATE);
	while (_running){
		std::this_thread::sleep_until(next_tick);
		if (!_running)
			break;
		// for (auto &it: _aggregates){
		// 	it.second.
		// }
		next_tick += std::chrono::seconds(TIME_TO_UPDATE);
	}
}

void IoAggregateMap::addWindow(size_t winTime, size_t intervalsec, size_t nbrBins){
	std::lock_guard<std::mutex> lock(_mutex);
	_aggregates[winTime] = std::make_unique<IoAggregate>(winTime, intervalsec, nbrBins);
}

bool IoAggregateMap::containe(size_t winTime) const {return (_aggregates.find(winTime) != _aggregates.end());}

const IoMap& IoAggregateMap::getIoMap() const{return _map;}

std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator IoAggregateMap::begin(){return _map.begin();}
std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator IoAggregateMap::end(){return _map.end();}

std::ostream& operator<<(std::ostream &os, const IoAggregateMap &other){
	os << other.getIoMap() << std::endl << std::endl;
	return os;
}
