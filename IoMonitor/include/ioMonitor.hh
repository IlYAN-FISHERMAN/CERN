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

struct IoMark {
	struct timespec io_time;
    size_t bytes;

    IoMark(size_t bytes) : bytes(bytes){
		clock_gettime(CLOCK_REALTIME, &io_time);
	}

    IoMark() : bytes(0){
		clock_gettime(CLOCK_REALTIME, &io_time);
	}
};

const char*	getCurrentTime();

namespace config {
	constexpr bool IoStatDebug = false;
	constexpr bool IoMapDebug = false;
	constexpr bool IoMarkDebug = false;
}
