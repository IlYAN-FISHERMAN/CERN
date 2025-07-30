//  File: ioMonitor.hh
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

#include <chrono>
#include <deque>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <mutex>
#include <optional>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <cstdint>
#include <sys/types.h>
#include <cmath>
#include <iostream>
#include <set>
#include <ctime>
#include <string.h>
#include <map>
#include <unistd.h>
#include "color.hh"


//--------------------------------------------
/// Main structure stored in IoStat
/// It keeps its timestamp when called
/// by any contructor
//--------------------------------------------
struct IoMark {
	struct timespec io_time;
    size_t bytes;

	//--------------------------------------------
	/// Main contructor
	//--------------------------------------------
    IoMark(size_t bytes) : bytes(bytes){
		clock_gettime(CLOCK_REALTIME, &io_time);
	}

	//--------------------------------------------
	/// Default contructor
	//--------------------------------------------
    IoMark() : bytes(0){
		clock_gettime(CLOCK_REALTIME, &io_time);
	}
};


//--------------------------------------------
/// @brief Get the current time
///
/// @return const char* the current time
//--------------------------------------------
const char*	getCurrentTime();


//--------------------------------------------
/// Namespace for debugging
///
/// if debug is "true" all debug phases of
/// the corresponding class will be displayed
//--------------------------------------------
namespace config {
	constexpr bool IoStatDebug = false;
	constexpr bool IoMapDebug = true;
	constexpr bool IoMarkDebug = false;
}
