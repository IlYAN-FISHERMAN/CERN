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

class IoAggregate {
	private:
		struct Bin {
			std::unordered_map<std::string, IoStatSummary> appStats;
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
		~IoAggregate();

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

		void addSample(const std::string &app, const IoStatSummary &summary);

		std::unordered_map<std::string, IoStatSummary> getCurrentMapAggregated() const;
		std::optional<IoStatSummary> getAggregated() const;
};
