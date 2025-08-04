//  File: ioAggregate.hh
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

#include "ioMap.hh"

//--------------------------------------------
/// The current name of the class when us
/// printInfo function
//--------------------------------------------
#define IOAGGREGATE_NAME "IoAggregate"

class IoAggregate {
	private:
		struct Bin {
			std::unordered_map<std::string, IoStatSummary> appStats;
		};

		size_t _numBins;
		size_t _intervalSec;
		size_t _currentIndex;
		std::vector<Bin> _bins;
		mutable std::mutex _mutex;

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
		IoAggregate(const IoStat &other);

		//--------------------------------------------
		/// Overload the operator =
		//--------------------------------------------
		IoAggregate& operator=(const IoAggregate &other);

		IoAggregate(size_t numBins, size_t intervalSec);

		void addSample(const std::string &app, const IoStatSummary &summary);
		std::optional<IoStatSummary> getAggregated(const std::string &app, size_t seconds) const;
		void shiftWindow();

};

class AggregatedIoMap {
	private:
		void aggregationLoop();
		size_t computeMaxIntervalSec() const;

		IoMap _base;
		std::unordered_map<size_t, std::unique_ptr<IoAggregate>> _aggregates;

		std::thread _aggregationThread;
		std::atomic<bool> _running;
	public:
		explicit AggregatedIoMap(const std::vector<size_t> &aggregationWindows);
		~AggregatedIoMap();

	void addRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes);
	void addWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes);

	std::optional<IoStatSummary> getBandwidth(const std::string& app, size_t seconds);
	std::optional<IoStatSummary> getBandwidth(uid_t uid, size_t seconds);
	
	std::vector<size_t> getAvailableWindows() const;
};

