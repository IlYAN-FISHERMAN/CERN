//  File: ioAggregate.cc
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

#include "../include/ioAggregate.hh"

IoAggregate::IoAggregate(size_t winDuration, size_t intervalSec){
	if (intervalSec)
	_numBins(winDuration);_intervalSec(intervalSec);_currentIndex(0);_bins(numBins);
}

IoAggregate::~IoAggregate(){}

void IoAggregate::addSample(const std::string &app, const IoStatSummary &summary){
	std::lock_guard<std::mutex> lock(_mutex);

	if (_bins.empty()){
		_bins.emplace_back(Bin(app, summary));
	}
	else if (_currentIndex < _bins.size()){
		_bins.at(_currentIndex).appStats.insert({app, summary});
	}
}

IoAggregate::IoAggregate(const IoAggregate &other){
	std::lock_guard<std::mutex> lock(_mutex);
	_numBins = other._numBins;
	_intervalSec = other._intervalSec;
	_currentIndex = other._currentIndex;
	_bins = other._bins;
}

IoAggregate& IoAggregate::operator=(const IoAggregate &other){
	std::lock_guard<std::mutex> lock(_mutex);
	if (this != &other){
		_numBins = other._numBins;
		_intervalSec = other._intervalSec;
		_currentIndex = other._currentIndex;
		_bins = other._bins;
	}
	return *this;
}

std::optional<IoStatSummary> IoAggregate::getAggregated(const std::string &app, size_t seconds) const{
	std::lock_guard<std::mutex> lock(_mutex);
	std::optional<IoStatSummary> value;

	// for (auto &it : _bins)
	// 	if (it)
	(void)app;
	(void)seconds;
	return value;
}

void IoAggregate::shiftWindow(){}

