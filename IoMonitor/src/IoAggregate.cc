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

IoAggregate::IoAggregate(size_t winTime)
	: _intervalSec(10), _currentIndex(0) ,_currentTime(std::chrono::system_clock::now()){
	if (winTime < 10)
		winTime = 10;
	if (winTime % _intervalSec != 0)
		winTime -= winTime % _intervalSec;
	_winTime = winTime;
	_bins.emplace_back(Bin());
}

void IoAggregate::update(const IoMap &maps){
	auto diff = std::chrono::system_clock::now() - _currentTime;

	if (diff >= std::chrono::seconds(_intervalSec)){
		printInfo(std::cout, "updating...");
		if constexpr (io::IoAggregateDebug)
			printInfo(std::cout, "updating...");
		for (auto it = _apps.begin(); it != _apps.end(); it++){
			auto summary = maps.getSummary(*it, _intervalSec);
			if (summary.has_value())
				addSample(*it, summary.value());
			else if constexpr (io::IoAggregateDebug)
				printInfo(std::cout, "No summary to add for " + std::string(*it));
		}
		for (auto it = _uids.begin(); it != _uids.end(); it++){
			auto summary = maps.getSummary(io::TYPE::UID, *it, _intervalSec);
			if (summary.has_value())
				addSample(io::TYPE::UID, *it, summary.value());
			else if constexpr (io::IoAggregateDebug)
				printInfo(std::cout, "No summary to add for " + std::to_string(*it));
		}
		for (auto it = _gids.begin(); it != _gids.end(); it++){
			auto summary = maps.getSummary(io::TYPE::GID, *it, _intervalSec);
			if (summary.has_value())
				addSample(io::TYPE::GID, *it, summary.value());
			else if constexpr (io::IoAggregateDebug)
				printInfo(std::cout, "No summary to add for " + std::to_string(*it));
		}
		_currentTime = std::chrono::system_clock::now();
	}
}

void IoAggregate::shiftWindow(){
	_currentIndex++;
	_bins.emplace_back(Bin());
}

std::optional<IoStatSummary> IoAggregate::summaryWeighted(std::vector<IoStatSummary> summarys) const{
	size_t rDivisor = 0;
	size_t wDivisor = 0;
	IoStatSummary weighted;

	if (io::IoAggregateDebug)
		printInfo(std::cout, "summary weighted called");

	for (const auto &it : summarys){
		if (it.readBandwidth.has_value())
			weighted.readBandwidth->first += (it.readBandwidth->first * it.rSize);
		if (it.writeBandwidth.has_value())
			weighted.writeBandwidth->first += (it.writeBandwidth->first * it.wSize);
		rDivisor += it.rSize;
		wDivisor += it.wSize;
	}

	if (rDivisor > 0)
		weighted.readBandwidth->first /= rDivisor;
	if (wDivisor > 0)
		weighted.writeBandwidth->first /= wDivisor;

	for (const auto &it : summarys){
		if (weighted.readBandwidth.has_value())
			weighted.readBandwidth->second += (it.rSize * \
				(std::pow(it.readBandwidth->second, 2) + std::pow(it.readBandwidth->first - \
													  weighted.readBandwidth->first, 2)));
		if (weighted.writeBandwidth.has_value())
			weighted.writeBandwidth->second += (it.wSize * \
				(std::pow(it.writeBandwidth->second, 2) + std::pow(it.writeBandwidth->first - \
													  weighted.writeBandwidth->first, 2)));
	}

	if (rDivisor > 0 && weighted.readBandwidth.has_value())
			weighted.readBandwidth->second = std::sqrt(weighted.readBandwidth->second / rDivisor);

	if (wDivisor > 0 && weighted.writeBandwidth.has_value())
			weighted.writeBandwidth->second = std::sqrt(weighted.writeBandwidth->second / wDivisor);


	weighted.rSize = rDivisor;
	weighted.wSize = wDivisor;

	if (io::IoAggregateDebug)
		printInfo(std::cout, "summary weighted succeeded");
	if (weighted.wSize == 0 && weighted.rSize == 0)
		return std::nullopt;
	if (weighted.rSize == 0)
		weighted.readBandwidth = std::nullopt;
	if (weighted.wSize == 0)
		weighted.writeBandwidth = std::nullopt;
	return weighted;
}


std::ostream& operator<<(std::ostream &os, const IoAggregate &other){
	std::lock_guard<std::mutex> lock(other._mutex);
	os << C_GREEN << "[" << C_CYAN << "IoAggregate" << C_GREEN << "]" << C_RESET << std::endl;
	os << C_GREEN << "[" << C_YELLOW << "window time: " << other._winTime << C_GREEN << "]" << C_RESET;
	os << C_GREEN << "[" << C_YELLOW << "interval/win: " << other._intervalSec << C_GREEN << "]" << C_RESET;
	os << C_GREEN << "[" << C_YELLOW << "nbr of bin: " << other._bins.size() << C_GREEN << "]" << C_RESET;
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
	os << std::fixed << std::setprecision(4);

	auto it = other._bins.at(other._currentIndex);
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

	os << C_RESET;
	return os;
}

//--------------------------------------------
/// Display the string given as parameter in
/// specific format with the current time
//--------------------------------------------
void	IoAggregate::printInfo(std::ostream &os, const char *msg) const{
	const char *time = getCurrentTime();
	os << IOAGGREGATE_NAME << " [" << time << "]: " << msg << std::endl;
}

//--------------------------------------------
/// Display the string given as parameter in
/// specific format with the current time
//--------------------------------------------
void	IoAggregate::printInfo(std::ostream &os, const std::string &msg) const{
	const char *time = getCurrentTime();
	os << IOAGGREGATE_NAME << " [" << time << "]: " << msg << std::endl;
}
