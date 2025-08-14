//  File: IoAggregate.cc
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

#include "../include/IoAggregate.hh"

IoAggregate::IoAggregate(const IoAggregate &other){
	std::lock_guard<std::mutex> lock(_mutex);
	std::lock_guard<std::mutex> otherLock(other._mutex);
	_nbrBins = other._nbrBins;
	_intervalSec = other._intervalSec;
	_currentIndex = other._currentIndex;
	_bins = other._bins;
	_currentTime = other._currentTime;
	_apps = other._apps;
	_uids = other._uids;
	_gids = other._gids;
}

IoAggregate& IoAggregate::operator=(const IoAggregate &other){
	std::lock_guard<std::mutex> lock(_mutex);
	std::lock_guard<std::mutex> otherLock(other._mutex);
	if (this != &other){
		_nbrBins = other._nbrBins;
		_intervalSec = other._intervalSec;
		_currentIndex = other._currentIndex;
		_bins = other._bins;
		_currentTime = other._currentTime;
		_apps = other._apps;
		_uids = other._uids;
		_gids = other._gids;
	}
	return *this;
}

IoAggregate::IoAggregate(size_t winTime, size_t intervalSec, size_t nbrBins)
	: _currentIndex(0), _currentTime(std::chrono::system_clock::now()){
	if (winTime < 60)
		winTime = 60;
	if (intervalSec == 0)
		intervalSec = 1;
	if (intervalSec > winTime)
		intervalSec = winTime;
	if (winTime % intervalSec != 0)
		winTime -= winTime % intervalSec;
	_winTime = winTime;
	_intervalSec = intervalSec;
	nbrBins == 0 ? _nbrBins = 1 : _nbrBins = nbrBins;
	_bins.resize(nbrBins);
}

void IoAggregate::update(const IoMap &maps){
	auto diff = std::chrono::system_clock::now() - _currentTime;

	if (diff >= std::chrono::seconds(_intervalSec)){
		for (auto it = _apps.begin(); it != _apps.end(); it++){
			auto summary = maps.getSummary(*it, _intervalSec);
			if (summary.has_value())
				addSample(*it, summary.value());
		}
		for (auto it = _uids.begin(); it != _uids.end(); it++){
			auto summary = maps.getSummary(io::TYPE::UID, *it, _intervalSec);
			if (summary.has_value())
				addSample(io::TYPE::UID, *it, summary.value());
		}
		for (auto it = _gids.begin(); it != _gids.end(); it++){
			auto summary = maps.getSummary(io::TYPE::GID, *it, _intervalSec);
			if (summary.has_value())
				addSample(io::TYPE::GID, *it, summary.value());
		}
		_currentTime = std::chrono::system_clock::now();
	}
}

void IoAggregate::shiftWindow(){
	_currentIndex++;
	if (_currentIndex >= _bins.size())
		_currentIndex = 0;
	_bins.e
}

std::ostream& operator<<(std::ostream &os, const IoAggregate &other){
	std::lock_guard<std::mutex> lock(other._mutex);
	os << C_GREEN << "[" << C_CYAN << "IoAggregate" << C_GREEN << "]" << C_RESET << std::endl;
	os << C_GREEN << "[" << C_YELLOW << "window time: " << other._winTime << C_GREEN << "]" << C_RESET;
	os << C_GREEN << "[" << C_YELLOW << "interval/win: " << other._intervalSec << C_GREEN << "]" << C_RESET;
	os << C_GREEN << "[" << C_YELLOW << "nbr of bin: " << other._bins.size() << "/" << other._nbrBins << C_GREEN << "]" << C_RESET;
	os << C_GREEN << "[" << C_YELLOW << "currentIndex: " << other._currentIndex << C_GREEN << "]" << C_RESET << std::endl;
	
	os << C_BLUE;
	os << "\t[Tracks]" << std::endl;
	os << "\t apps:" << std::endl;
	for (auto it : other._apps)
		os << "\t  - " << it << std::endl;
	os << "\t uids:" << std::endl;
	for (auto it : other._uids)
		os << "\t  - " << it << std::endl;
	os << "\t gids:" << std::endl;
	for (auto it : other._gids)
		os << "\t  - " << it << std::endl;
	os << std::endl;


	auto it = other._bins.at(other._currentIndex);
	if (it.appStats.size() == 0 && it.uidStats.size() == 0 && it.gidStats.size() == 0)
		os << "[empty]" << std::endl;
	else
		os << "[binsNbr: " << other._currentIndex << "]"<< std::endl << std::endl;
	if (it.appStats.size() > 0){
		os << "apps:" << std::endl;
		for (auto apps : it.appStats)
			os << "\t[" << apps.first << "]" << std::endl << "\t- " << apps.second << std::endl;
	}
	if (it.uidStats.size() > 0){
		os << "uids:" << std::endl;
		for (auto uids : it.uidStats)
			os << "\t[" << uids.first << "]" << std::endl << "\t- " << uids.second << std::endl;
	}
	if (it.gidStats.size() > 0){
		os << "gids:" << std::endl;
		for (auto gids : it.gidStats)
			os << "\t[" << gids.first << "]" << std::endl << "\t- " << gids.second << std::endl;
	}

	os << C_RESET << std::endl;
	return os;
}
