//  File: ioAggregateMap.hh
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

#include "ioAggregate.hh"

//--------------------------------------------
/// The current name of the class when us
/// printInfo function
//--------------------------------------------
#define IOAGGREGATEMAP_NAME "IoAggregateMap"

class IoAggregateMap {
	private:
		void aggregationLoop();
		size_t computeMaxIntervalSec() const;

		IoMap _map;
		std::unordered_map<size_t, std::unique_ptr<IoAggregate>> _aggregates;

		std::thread _thread;
		std::atomic<bool> _running;

		IoAggregateMap() = delete;

	public:
		IoAggregateMap(const IoAggregateMap &other) = delete;
		IoAggregateMap& operator=(const IoAggregateMap &other) = delete;

		~IoAggregateMap();

		IoAggregateMap(const std::vector<size_t> &aggregationWindows);
		IoAggregateMap(const std::vector<size_t> &aggregationWindows, int);

		void addRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes);
		void addWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes);

		std::vector<size_t> getAvailableWindows() const;

		template <typename T>
		std::optional<IoStatSummary> getBandwidth(T index, size_t seconds, IoStat::Marks enumMark){
			return (_map.getBandwidth(index, enumMark, seconds));
		}
	
};
