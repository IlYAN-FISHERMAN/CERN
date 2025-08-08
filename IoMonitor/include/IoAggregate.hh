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

class IoAggregateBase {
	public:
		virtual ~IoAggregateBase() = 0;

		virtual void addSample(const std::string &app, const IoStatSummary &summary) = 0;

		virtual std::unordered_map<std::string, IoStatSummary> getCurrentMapAggregated() const = 0;

		virtual std::optional<IoStatSummary> getAggregated() const = 0;
};

template <typename T>
class IoAggregate : public IoAggregateBase {
	private:
		struct Bin {
			std::unordered_map<T, IoStatSummary> appStats;
		};

		size_t _nbrBins;
		size_t _intervalSec;
		size_t _winTime;
		size_t _currentIndex;

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
		IoAggregate(const IoAggregate &other){
			std::lock_guard<std::mutex> lock(_mutex);
			std::lock_guard<std::mutex> otherLock(other._mutex);
			_nbrBins = other._nbrBins;
			_intervalSec = other._intervalSec;
			_currentIndex = other._currentIndex;
			_bins = other._bins;
	}

		//--------------------------------------------
		/// Overload the operator =
		//--------------------------------------------
		IoAggregate& operator=(const IoAggregate &other){
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

		//--------------------------------------------
		/// Main constructor
		//--------------------------------------------
		explicit IoAggregate(size_t winTime, size_t intervalSec, size_t nbrBins) : _currentIndex(0){
			if (intervalSec == 0)
				intervalSec = 1;
			else if (intervalSec > winTime)
				intervalSec = winTime;
			if (winTime < 60)
				winTime = 60;
			if (winTime % intervalSec != 0)
				winTime -= winTime % intervalSec;
			_winTime = winTime;
			_intervalSec = intervalSec;
			nbrBins == 0 ? _nbrBins = 1 : _nbrBins = nbrBins;
			_bins.resize(nbrBins);
		}

		void addSample(const std::string &app, const IoStatSummary &summary) override{
			std::lock_guard<std::mutex> lock(_mutex);
				if (_currentIndex < _bins.size()){
					if (_bins.at(_currentIndex).appStats.size() == _winTime / _intervalSec){
						_currentIndex++;
						if (_currentIndex > _nbrBins){
							_currentIndex = 0;
							_bins.at(_currentIndex).appStats.clear();
						}
					}
					_bins.at(_currentIndex).appStats.emplace(app, summary);
				}
		}

		std::unordered_map<std::string, IoStatSummary> getCurrentMapAggregated() const override{ return (_bins.at(_currentIndex).appStats);}

		std::optional<IoStatSummary> getAggregated() const override{
			std::lock_guard<std::mutex> lock(_mutex);

			IoStatSummary summary;
			if (_bins.at(_currentIndex).appStats.size() == 0)
				return std::nullopt;

		}

};
