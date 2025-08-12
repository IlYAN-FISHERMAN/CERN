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
#include <variant>
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
		explicit IoAggregate(size_t winTime, size_t intervalSec, size_t nbrBins);

		friend std::ostream& operator<<(std::ostream &os, const IoAggregate &other);

		template <typename T>
		void setTrack(T index){
			if (std::is_same_v<T, gid_t>)
				std::cout << "gid" << std::endl;
			if (std::is_same_v<T, uid_t>)
				std::cout << "uid" << std::endl;
			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char *>)
				_apps.insert(std::string(index));
			else if (std::is_same_v<T, uid_t>)
				_uids.insert(index);
			else if (std::is_same_v<T, gid_t>)
				_gids.insert(index);
			else
				return;
		}

		template <typename T>
		void addSample(const T index, IoStatSummary &summary){
			std::lock_guard<std::mutex> lock(_mutex);

			if (_currentIndex > _bins.size())
				_currentIndex = 0;
			if (std::is_same_v<T, std::string> || std::is_same_v<T, const char *>){
				if (_bins.at(_currentIndex).appStats.size() == _winTime / _intervalSec){
					_currentIndex++;
					if (_currentIndex > _nbrBins){
						_currentIndex = 0;
						_bins.at(_currentIndex).appStats.clear();
					}
				}
				_bins.at(_currentIndex).appStats.insert({std::string(index), summary});
			}
			else if (std::is_same_v<T, uid_t>){
				if (_bins.at(_currentIndex).uidStats.size() == _winTime / _intervalSec){
					_currentIndex++;
					if (_currentIndex > _nbrBins){
						_currentIndex = 0;
						_bins.at(_currentIndex).uidStats.clear();
					}
				}
				_bins.at(_currentIndex).uidStats.insert({index, summary});
			}
			else if (std::is_same_v<T, gid_t>){
				if (_bins.at(_currentIndex).gidStats.size() == _winTime / _intervalSec){
					_currentIndex++;
					if (_currentIndex > _nbrBins){
						_currentIndex = 0;
						_bins.at(_currentIndex).gidStats.clear();
					}
				}
				_bins.at(_currentIndex).gidStats.insert({index, summary});
			}
		}
};
