//  File: IoAggregateMap.hh
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

#include "IoAggregate.hh"

#define TIME_TO_UPDATE 1

//--------------------------------------------
/// The current name of the class when us
/// printInfo function
//--------------------------------------------
#define IOAGGREGATEMAP_NAME "IoAggregateMap"

class IoAggregateMap{
	private:
		void updateAggregateLoop();

		IoMap _map;
		std::unordered_map<size_t, std::unique_ptr<IoAggregate> > _aggregates;

		std::thread _thread;
		std::atomic<bool> _running;
		std::condition_variable _cv;
		
		mutable std::mutex _mutex;

	public:

		IoAggregateMap();
		IoAggregateMap(int);

		IoAggregateMap(const IoAggregateMap &other) = delete;
		IoAggregateMap& operator=(const IoAggregateMap &other) = delete;

		~IoAggregateMap();


		void addRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes);
		void addWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes);
		int addWindow(size_t winTime);

		//--------------------------------------------
		/// Display the string given as parameter in
		/// specific format with the current time
		//--------------------------------------------
		void printInfo(std::ostream &os, const char *msg) const;

		//--------------------------------------------
		/// Display the string given as parameter in
		/// specific format with the current time
		//--------------------------------------------
		void printInfo(std::ostream &os, const std::string &msg) const;

		std::optional<std::vector<size_t> > getAvailableWindows() const;
		const IoMap& getIoMap() const;

		template <typename T>
		int setTrack(size_t winTime, const T index){
			if constexpr (io::IoAggregateMapDebug)
				printInfo(std::cout, "set appName track for " + std::string(index));
			if (!this->containe(winTime)){
				if constexpr (io::IoAggregateMapDebug)
					printInfo(std::cerr, "set appName track failed");
				return -1;
			}
			_aggregates[winTime]->setTrack(index);
			if constexpr (io::IoAggregateMapDebug)
				printInfo(std::cout, "set appName track succeeded");
			return 0;
		}

		template <typename T>
		int setTrack(size_t winTime, io::TYPE type, const T index){
			if constexpr (io::IoAggregateMapDebug)
				printInfo(std::cout, "set id track for " + std::to_string(index));
			if (!this->containe(winTime)){
				if constexpr (io::IoAggregateMapDebug)
					printInfo(std::cerr, "set id track failed");
				return -1;
			}
			_aggregates[winTime]->setTrack(type, index);
			if constexpr (io::IoAggregateMapDebug)
				printInfo(std::cout, "set id track succeeded");
			return 0;
		}

		bool containe(size_t winTime) const;

		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator begin();
		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator end();

		friend std::ostream& operator<<(std::ostream &os, const IoAggregateMap &other);

		void shiftWindow(size_t winTime);
		void shiftWindow(size_t winTime, size_t index);

		template<typename T>
		std::optional<IoStatSummary> getSummary(size_t winTime, const T index){
			std::lock_guard<std::mutex> lock(_mutex);
			if (_aggregates.find(winTime) == _aggregates.end())
				return std::nullopt;
			return (_aggregates[winTime]->getSummary(index));
		}

		template<typename T>
		std::optional<IoStatSummary> getSummary(size_t winTime, io::TYPE type, const T index){
			std::lock_guard<std::mutex> lock(_mutex);
			if (_aggregates.find(winTime) == _aggregates.end())
				return std::nullopt;
			return (_aggregates[winTime]->getSummary(type, index));
		}
};
