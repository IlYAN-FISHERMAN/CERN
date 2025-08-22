//  File: IoAggregate.hh
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

//--------------------------------------------
/// Each class has a variable define DEBUG which
/// can be defined in the IoMonitor.hh namespace
//--------------------------------------------

#pragma once

#include "IoMap.hh"

//--------------------------------------------
/// The current name of the class when us
/// printInfo function
//-----------------------------------------
#define IOAGGREGATE_NAME "IoAggregate"

class IoAggregate{
	private:

		//-----------------------------------------
		/// Structure that keeps all IoStatSummary
		/// of all appName/uid/gid that are tracked
		//-----------------------------------------
		struct Bin{
			std::unordered_multimap<std::string, IoStatSummary> appStats;
			std::unordered_multimap<uid_t, IoStatSummary> uidStats;
			std::unordered_multimap<gid_t, IoStatSummary> gidStats;
		};

		size_t _nbrBins;
		size_t _intervalSec;
		size_t _winTime;
		size_t _currentIndex;
		std::chrono::system_clock::time_point _currentTime;

		std::unordered_set<std::string>	_apps;
		std::unordered_set<uid_t>		_uids;
		std::unordered_set<gid_t>		_gids;
 
		std::vector<Bin> _bins;

		mutable std::mutex _mutex;

		//--------------------------------------------
		/// Deleted default constructor
		//--------------------------------------------
		IoAggregate() = delete;

		//--------------------------------------------
		/// Display the string given as parameter in
		/// specific format with the current time
		//--------------------------------------------
		void printInfo(std::ostream &os, const std::string &msg) const;

		//--------------------------------------------
		/// Display the string given as parameter in
		/// specific format with the current time
		//--------------------------------------------
		void printInfo(std::ostream &os, const char *msg) const;

	public:
		//--------------------------------------------
		/// Orthodoxe canonical form
		//--------------------------------------------

		//--------------------------------------------
		/// Destructor
		//--------------------------------------------
		~IoAggregate(){}

		//--------------------------------------------
		/// Constructor by copy constructor
		//--------------------------------------------
		IoAggregate(const IoAggregate &other);

		//--------------------------------------------
		/// Overload the operator =
		//--------------------------------------------
		IoAggregate& operator=(const IoAggregate &other);

		//--------------------------------------------
		/// Main constructor
		//--------------------------------------------
		explicit IoAggregate(size_t winTime);

		void update(const IoMap &maps);
		void shiftWindow();
		void shiftWindow(size_t index);


		friend std::ostream& operator<<(std::ostream &os, const IoAggregate &other);
		
		std::optional<IoStatSummary> summaryWeighted(std::vector<IoStatSummary> summarys) const;

		template <typename T>
		int setTrack(T index){
			if (!(std::is_same_v<T, std::string> || std::is_same_v<T, const char *>))
				return -1;
			_apps.insert(std::string(index));
			return 0;
		}

		template <typename T>
		int setTrack(io::TYPE type, T index){
			if (type == io::TYPE::UID)
				_uids.insert(index);
			else if (type == io::TYPE::GID)
				_gids.insert(index);
			else
				return -1;
			return 0;
		}

		template <typename T>
		void addSample(io::TYPE type, const T index, IoStatSummary &summary){

			if constexpr (io::IoAggregateDebug)
				printInfo(std::cout, "add sample for " + std::to_string(index));
			if (type == io::TYPE::UID){
				auto &uid = _bins.at(_currentIndex).uidStats;
				auto range = uid.equal_range(index);
				if (static_cast<size_t>(std::distance(range.first, range.second)) >= _winTime / _intervalSec){
					struct timespec now;
					clock_gettime(CLOCK_REALTIME, &now);
					for (auto it = range.first; range.first != range.second;){
						if (difftime(now.tv_sec, range.first->second.io_time.tv_sec) > difftime(now.tv_sec, it->second.io_time.tv_sec))
							it = range.first;
						range.first++;
						if (range.first == range.second)
							uid.erase(it);
					}
				}
				uid.insert({index, summary});
				if constexpr (io::IoAggregateDebug)
					printInfo(std::cout, "add uid sample succeedded");
			}
			else if (type == io::TYPE::GID){
				auto &gid = _bins.at(_currentIndex).gidStats;
				auto range = gid.equal_range(index);
				if (static_cast<size_t>(std::distance(range.first, range.second)) >= _winTime / _intervalSec){
					struct timespec now;
					clock_gettime(CLOCK_REALTIME, &now);
					for (auto it = range.first; range.first != range.second;){
						if (difftime(now.tv_sec, range.first->second.io_time.tv_sec) > difftime(now.tv_sec, it->second.io_time.tv_sec))
							it = range.first;
						range.first++;
						if (range.first == range.second)
							gid.erase(it);
					}
				}
				gid.insert({index, summary});
				if constexpr (io::IoAggregateDebug)
					printInfo(std::cout, "add gid sample succeedded");
			}
		}

		template <typename T>
		void addSample(const T index, IoStatSummary &summary){

			if constexpr (io::IoAggregateDebug)
				printInfo(std::cout, "add Sample for " + std::string(index));
			if (std::is_same_v<T, std::string> || std::is_same_v<T, const char *>){
				auto &app = _bins.at(_currentIndex).appStats;
				auto range = app.equal_range(index);
				if (static_cast<size_t>(std::distance(range.first, range.second)) >= _winTime / _intervalSec){
					struct timespec now;
					clock_gettime(CLOCK_REALTIME, &now);
					for (auto it = range.first; range.first != range.second;){
						if (difftime(now.tv_sec, range.first->second.io_time.tv_sec) > difftime(now.tv_sec, it->second.io_time.tv_sec))
							it = range.first;
						range.first++;
						if (range.first == range.second)
							app.erase(it);
					}
				}
				app.insert({std::string(index), summary});
				if constexpr (io::IoAggregateDebug)
					printInfo(std::cout, "add app sample succeedded");
			}
		}

		template <typename T>
		std::optional<IoStatSummary> getSummary(const T index){
			std::vector<IoStatSummary> summarys;

			if constexpr (io::IoAggregateDebug)
				printInfo(std::cout, "get Summary for " + std::string(index));
			if (!(std::is_same_v<T, std::string> || std::is_same_v<T, const char *>)
			|| _apps.find(index) == _apps.end())
				return std::nullopt;
			auto &it = _bins.at(_currentIndex);
			for (auto appsSumarrys : it.appStats)
				if (appsSumarrys.first == index)
					summarys.emplace_back(appsSumarrys.second);

			if constexpr (io::IoAggregateDebug)
				printInfo(std::cout, "get Summary succeeded");
			return summaryWeighted(summarys);
		}

		template <typename T>
		std::optional<IoStatSummary> getSummary(io::TYPE type, const T index){
			std::vector<IoStatSummary> summarys;

			if (type != io::TYPE::GID && type != io::TYPE::UID)
				return std::nullopt;

			if ((type == io::TYPE::UID && _uids.find(index) == _uids.end())
			|| (type == io::TYPE::GID && _gids.find(index) == _gids.end()))
				return std::nullopt;
			
			auto &it = _bins.at(_currentIndex);
			if (type == io::TYPE::UID)
				for (auto uidsSumarrys : it.uidStats){
					if (uidsSumarrys.first == static_cast<uid_t>(index))
						summarys.emplace_back(uidsSumarrys.second);
			}
			else if (type == io::TYPE::GID)
				for (auto gidsSumarrys : it.gidStats)
					if (gidsSumarrys.first == static_cast<gid_t>(index))
						summarys.emplace_back(gidsSumarrys.second);

			return summaryWeighted(summarys);
		}
};
