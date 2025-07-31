//  File: ioMap.hh
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

#include "ioStat.hh"
#include <iomanip>
#include <numeric>

//--------------------------------------------
/// The current name of the class when us
/// printInfo function
//--------------------------------------------
#define IOMAP_NAME "IoMap"

//--------------------------------------------
/// the time the cleanloop function must wait
/// before cleaning the map
//--------------------------------------------
#define TIME_TO_CLEAN 60

class IoMap {
	private:
		//--------------------------------------------
		/// @brief Multithreaded function to clean up
		/// inactive IoStats
		///
		/// @details
		/// Multithreaded function that checks for
		/// inactive I/Os every 60 seconds and removes them
		//--------------------------------------------
		void cleanerLoop();

		//--------------------------------------------
		/// @brief Displays the string given as a parameter
		/// in a format corresponding to the class with the
		/// current timestamp
		///
		/// @param	os The output stream
		/// @param	msg The message to display
		/// - Exemple: std::cout/std::cerr
		//--------------------------------------------
		void	printInfo(std::ostream &os, const char *);

		//--------------------------------------------
		/// @brief Displays the string given as a parameter
		/// in a format corresponding to the class with the
		/// current timestamp
		///
		/// @param	os The output stream
		/// @param	msg The message to display
		/// - Exemple: std::cout/std::cerr
		//--------------------------------------------
		void	printInfo(std::ostream &os, const std::string &);

		//--------------------------------------------
		/// Main variable that keeps track of all IoStats
		//--------------------------------------------
		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > _filesMap;

		//--------------------------------------------
		/// Keeps all app name
		//--------------------------------------------
		std::unordered_set<std::string> _apps;

		//--------------------------------------------
		/// Keeps all users id
		//--------------------------------------------
		std::unordered_set<uid_t> _uids;

		//--------------------------------------------
		/// Keeps all groups id
		//--------------------------------------------
		std::unordered_set<gid_t> _gids;

		//--------------------------------------------
		/// Variables to manage multithreading
		//--------------------------------------------
		mutable std::mutex _mutex;
		std::thread _cleaner;
		std::atomic<bool> _running;
		std::condition_variable _cv;

	public:
		//--------------------------------------------
		/// Orthodoxe canonical form
		//--------------------------------------------

		//--------------------------------------------
		/// Main constructor
		//--------------------------------------------
		IoMap();

		//--------------------------------------------
		/// Destructor
		//--------------------------------------------
		~IoMap();

		//--------------------------------------------
		/// Explicite block constructor by copy constructor
		//--------------------------------------------
		IoMap(const IoMap &other) = delete;

		//--------------------------------------------
		/// Explicite block overload the operator =
		//--------------------------------------------
		IoMap& operator=(const IoMap &other) = delete;

		//--------------------------------------------
		/// @brief Optional constructor
		///
		/// @details
		/// If the constructor is called with any int,
		/// the multithreaded cleanLoop function will
		/// not be run, making debugging easier.
		///
		/// @param	int		ignored
		//--------------------------------------------
		IoMap(int);

		//--------------------------------------------
		/// Public static mutex to share outputs stream
		//--------------------------------------------
		static std::mutex _osMutex;
		
		//--------------------------------------------
		/// @brief adds an IoStat object to the multimap
		/// with the corresponding elements
		///
		/// @param	inode	File id
		/// @param	app		Name of the application
		/// @param uid		ID of the corresponding user
		/// @param gid		ID of the corresponding group
		/// @param rbytes	Number of bytes read
		//--------------------------------------------
		void AddRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes);

		//--------------------------------------------
		/// @brief adds an IoStat object to the multimap
		/// with the corresponding elements
		///
		/// @param	inode	file id
		/// @param	app		Name of the application
		/// @param uid		ID of the corresponding user
		/// @param gid		ID of the corresponding group
		/// @param rbytes	Number of bytes read
		//--------------------------------------------
		void AddWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes);


		//--------------------------------------------
		///@brief Get all apps
		///
		/// @return std::vector<std::string> Vector of
		/// all current active apps
		//--------------------------------------------
		std::vector<std::string> getApps() const;

		//--------------------------------------------
		///@brief Get all uids
		///
		/// @return std::vector<uid_t> Vector of
		/// all current active uids
		//--------------------------------------------
		std::vector<uid_t> getUids() const;

		//--------------------------------------------
		///@brief Get all gids
		///
		/// @return std::vector<uid_t> Vector of
		/// all current active gids
		//--------------------------------------------
		std::vector<gid_t> getGids() const;
		

		//--------------------------------------------
		///@brief Get a copy of the multimap
		///
		/// @return std::unordered_multimap<uint64_t, 
		/// std::shared_ptr<IoStat> > Vector of
		/// all current active gids
		//--------------------------------------------
		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > GetAllStatsSnapshot() const;

		//--------------------------------------------
		/// @brief Overload operator << to print
		/// the entire multimap from a IoMap object
		//--------------------------------------------
		friend std::ostream& operator<<(std::ostream &os, const IoMap &other);

		//--------------------------------------------
		/// @brief Overload operator << to print
		/// the entire multimap
		//--------------------------------------------
		friend std::ostream& operator<<(std::ostream &os, const std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > &other);

		//--------------------------------------------
		/// Template
		/// @brief Get the WRITE or READ bandwidth
		///
		/// @details
		/// The function calculates the READ or WRITE
		/// weighted bandwidth (depending on the enumMark
		/// variable given as a parameter) during the
		/// last N seconds depending on the index variable
		///
		/// @param	index Template variable
		/// index type can be const char*/std::string/uint_t/gid_t.
		/// Calculates the weighted bandwidth according to
		/// the type of the variable and get the data from
		/// all the corresponding I/Os
		/// @param	enumMark READ or WRITE variable comes
		/// from the IoStat::Marks enumerator
		/// - Exemple: IoStat::Marks::READ
		/// @param seconds(optional) The second range during
		/// the last N I/O from now (by default - 10s)
		///
		/// @return std::nullopt If an error is encountered
		/// @return std::optional<std::pair<double, double>>
		/// first is the weighted average, second is the
		/// weighted standard deviation 
		//--------------------------------------------
		template <typename T>
		std::optional<std::pair<double, double> > getBandwidth(const T index, IoStat::Marks enumMark, size_t seconds = 10){

		if (enumMark != IoStat::Marks::READ && enumMark != IoStat::Marks::WRITE)
			return std::nullopt;
		else if (seconds == 0)
			return (std::pair<double, double>(0, 0));

		std::map<std::pair<double, double>, size_t> indexData;
		std::pair<double, double> weighted = {0, 0};
		std::pair<double, double> tmp = {0, 0};
		size_t size = 0;

		/// Check the type of the index variable
		/// and after 
		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char *>){
			for (auto it : _filesMap){
				if (it.second->getApp() == index){
					tmp = it.second->bandWidth(enumMark, &size, seconds);
					indexData.insert({tmp, size});
					size = 0;
					tmp = {0, 0};
				}
			}
		} else if constexpr (std::is_same_v<T, uid_t>){
			for (auto it : _filesMap){
				if (it.second->getUid() == index){
					tmp = it.second->bandWidth(enumMark, &size, seconds);
					indexData.insert({tmp, size});
					size = 0;
					tmp = {0, 0};
				}
			}
		} else if constexpr (std::is_same_v<T, gid_t>){
			for (auto it : _filesMap){
				if (it.second->getGid() == index){
					tmp = it.second->bandWidth(enumMark, &size, seconds);
					indexData.insert({tmp, size});
					size = 0;
					tmp = {0, 0};
				}
			}
		}
		else{
			if (config::IoMapDebug)
				printInfo(std::cerr, "No match found for data type");
			return std::nullopt;
		}
		if (indexData.size() <= 0)
			return std::nullopt;

		/// Calcule weighted average/standard deviation
		size_t divisor = 0;
		size_t stdDivisor = 0;
		std::cout << std::fixed;
		for (const auto &it : indexData){
			weighted.first += (it.first.first * it.second);
			divisor += it.second;
			if (it.second > 1){
				stdDivisor += it.second - 1;
				weighted.second += (it.second - 1) * (std::pow(it.first.second, 2));
			}
		}
		if (divisor > 0)
			weighted.first /= divisor;
		if (stdDivisor > 0)
			weighted.second = std::sqrt(weighted.second / stdDivisor);

		return weighted;
	}
};
