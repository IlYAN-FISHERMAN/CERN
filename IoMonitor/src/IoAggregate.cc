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
}

IoAggregate& IoAggregate::operator=(const IoAggregate &other){
	std::lock_guard<std::mutex> lock(_mutex);
	std::lock_guard<std::mutex> otherLock(other._mutex);
	if (this != &other){
		_nbrBins = other._nbrBins;
		_intervalSec = other._intervalSec;
		_currentIndex = other._currentIndex;
		_bins = other._bins;
	}
	return *this;
}

IoAggregate::IoAggregate(size_t winTime, size_t intervalSec, size_t nbrBins) : _currentIndex(0){
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

std::ostream& operator<<(std::ostream &os, const IoAggregate &other){
	os << C_BLUE << "[IoAggregate]" << std::endl;
	
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

	os << C_RESET << std::endl;
	return os;
}
