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

#pragma once

#include "IoMap.hh"

//--------------------------------------------
/// The current name of the class when us
/// printInfo function
//-----------------------------------------
#define IOAGGREGATE_NAME "IoAggregate"

class IoAggregate{
	private:
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
		/// Default constructor
		//--------------------------------------------
		IoAggregate() = delete;

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

		friend std::ostream& operator<<(std::ostream &os, const IoAggregate &other);
		
		IoStatSummary summaryWeighted(std::vector<IoStatSummary> summarys) const;

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

			if (type == io::TYPE::UID)
				_bins.at(_currentIndex).uidStats.insert({index, summary});
			else if (type == io::TYPE::GID)
				_bins.at(_currentIndex).gidStats.insert({index, summary});
		}

		template <typename T>
		void addSample(const T index, IoStatSummary &summary){

			if (std::is_same_v<T, std::string> || std::is_same_v<T, const char *>)
				_bins.at(_currentIndex).appStats.insert({std::string(index), summary});
		}

		template <typename T>
		std::optional<IoStatSummary> getSummary(const T index){
			std::vector<IoStatSummary> summarys;

			if (!(std::is_same_v<T, std::string> || std::is_same_v<T, const char *>)
			|| (_apps.find(index) == _apps.end())
			|| _bins.at(_currentIndex).appStats.find(index) == _bins.at(_currentIndex).appStats.end())
				return std::nullopt;
			
			auto it = _bins.at(_currentIndex);
			for (auto appsSumarrys : it.appStats)
				summarys.emplace_back(appsSumarrys.second);
			return summaryWeighted(summarys);
		}

		template <typename T>
		std::optional<IoStatSummary> getSummary(io::TYPE type, const T index){
			std::vector<IoStatSummary> summarys;

			if (!(std::is_same_v<T, uid_t> || std::is_same_v<T, gid_t>)
			|| (type != io::TYPE::GID || type != io::TYPE::UID))
				return std::nullopt;

			if ((type == io::TYPE::UID && _uids.find(index) == _uids.end())
			|| (type == io::TYPE::GID && _gids.find(index) == _gids.end()))
				return std::nullopt;
			
			auto it = _bins.at(_currentIndex);
			if (type == io::TYPE::UID)
				for (auto uidsSumarrys : it.uidStats)
					summarys.emplace_back(uidsSumarrys.second);
			else if (type == io::TYPE::GID)
				for (auto gidsSumarrys : it.gidStats)
					summarys.emplace_back(gidsSumarrys.second);

			return summaryWeighted(summarys);
		}
};
