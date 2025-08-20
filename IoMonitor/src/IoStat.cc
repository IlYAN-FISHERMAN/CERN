//  File: IoStat.cc
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

#include "../include/IoStat.hh"

//--------------------------------------------
/// Get the current time
//--------------------------------------------
const char* getCurrentTime(){
	time_t timestamp;
	std::time(&timestamp);
	char *time = std::ctime(&timestamp);
	if (!time)
		exit (EXIT_FAILURE);
	time[strlen(time) - 1] = 0;
	return (time);
}

//--------------------------------------------
/// Operator = overloaded
//--------------------------------------------
IoStat& IoStat::operator=(const IoStat &other){
	if (this != &other){
		_fileId = other._fileId;
		_app = other._app;
		_uid = other._uid;
		_gid = other._gid;
		_readMarks = other._readMarks;
		_writeMarks = other._writeMarks;
	}
	return (*this);
}

//--------------------------------------------
/// Display the string given as parameter in
/// specific format with the current time
//--------------------------------------------
void	IoStat::printInfo(std::ostream &os, const char *msg){
	const char *time = getCurrentTime();
	os << IOSTAT_NAME << " [" << time << "]: " << msg << std::endl;
}

//--------------------------------------------
/// Display the string given as parameter in
/// specific format with the current time
//--------------------------------------------
void	IoStat::printInfo(std::ostream &os, const std::string &msg){
	const char *time = getCurrentTime();
	os << IOSTAT_NAME << " [" << time << "]: " << msg << std::endl;
}

//--------------------------------------------
/// Constructor by copy constructor
//--------------------------------------------
IoStat::IoStat(const IoStat &other) :
	_fileId(other._fileId), _app(other._app), _uid(other._uid),
	_gid(other._gid), _readMarks(other._readMarks), _writeMarks(other._writeMarks){
}

//--------------------------------------------
/// Main constructor
//--------------------------------------------
IoStat::IoStat(uint64_t fileId, const std::string& app, uid_t uid, gid_t gid) :
	_fileId(fileId), _app(app), _uid(uid), _gid(gid){
}

//--------------------------------------------
/// Destructor
//--------------------------------------------
IoStat::~IoStat(){}

//--------------------------------------------
/// Add bytes to the corresponding Read/Write deque
//--------------------------------------------
void IoStat::add(size_t bytes, IoStat::Marks enumMark){
	IoMark io(bytes);
	if (enumMark == Marks::READ)
		_readMarks.push_back(io);
	else if (enumMark == Marks::WRITE)
		_writeMarks.push_back(io);
}

//--------------------------------------------
/// Keep all I/O from the last N seconds
/// and cleans the rest
//--------------------------------------------
uint64_t IoStat::cleanOldsMarks(Marks enumMark, size_t seconds){
	if ((enumMark != Marks::READ && enumMark != Marks::WRITE)){
		if (io::IoStatDebug)
			IoStat::printInfo(std::cerr, "\033[031mNo marks found for\033[0m");
		return -1;
	}

	std::deque<IoMark> &mark = (enumMark == Marks::READ ? _readMarks : _writeMarks);

	std::deque<IoMark>::const_iterator begin = mark.begin();
	std::deque<IoMark>::const_iterator end = mark.end();

	struct timespec	currentTime;

	clock_gettime(CLOCK_REALTIME, &currentTime);

	// Calculates the iterator range corresponding to the last N seconds
	if (seconds > 0){
		for (std::deque<IoMark>::const_iterator it = begin; it != mark.end(); it++){
			if ((difftime(currentTime.tv_sec, it->io_time.tv_sec) < seconds))
				break;
			end = it;
		}
	}
	// checks if in case where seconds > 0 if there is something to delete
	if (seconds > 0 && end == mark.end()){
		if (io::IoStatDebug)
			printInfo(std::cout, "\033[031mNothing to clean\033[0m");
		return 1;
	}

	// Calculates the size of what will be erased
	size_t size = 0;
	if (seconds == 0){
		size = mark.size();
		mark.clear();
	} else{
		size = std::distance(begin, end);
		mark.erase(begin, end);
	}
	return size;
}

//--------------------------------------------
/// Calculate the write or read bandwidth
//--------------------------------------------
std::pair<double, double> IoStat::bandWidth(Marks enumMark, size_t *range, size_t seconds) const{
	if ((enumMark != Marks::READ && enumMark != Marks::WRITE) || seconds == 0){
		if constexpr (io::IoStatDebug){
			if (seconds == 0)
				IoStat::printInfo(std::cerr, "\033[031mCan't calculate bandwidth with 0s\033[0m");
			else
				IoStat::printInfo(std::cerr, "\033[031mNo marks found for\033[0m" + std::to_string(static_cast<uint8_t>(enumMark)));
		}
		return (std::pair(0, 0));
	}

	const std::deque<IoMark> &mark = (enumMark == Marks::READ ? _readMarks : _writeMarks);
	double avrg = 0;
	double deviation = 0;
	std::deque<IoMark>::const_iterator begin = mark.end();
	std::deque<IoMark>::const_iterator end = mark.end();
	struct timespec	currentTime;

	if (mark.empty())
		return (std::pair(0, 0));
	// Found the bandwidth iterator range
	clock_gettime(CLOCK_REALTIME, &currentTime);
	for (std::deque<IoMark>::const_iterator it = end; it != mark.begin();){
		it--;
		if ((difftime(currentTime.tv_sec, it->io_time.tv_sec) > seconds))
			break;
		begin = it;
	}
	if (range)
		*range = std::distance(begin, end);
	if (begin == end)
		return (std::pair(0, 0));

	// Calcule average
	for (std::deque<IoMark>::const_iterator it = begin; it < end; it++)
		avrg += static_cast<double>(it->bytes);
	if (std::distance(begin, end) > 0)
		avrg = avrg / std::distance(begin, end);

	// Calcule standard deviation
	if (std::distance(begin, end) - 1 > 0){
		for (std::deque<IoMark>::const_iterator it = begin; it < end; it++){
			deviation += std::pow(std::abs(it->bytes - avrg), 2);
		}
		deviation = std::sqrt(deviation / std::distance(begin, end));
	}

	return (std::pair<double, double>(avrg, deviation));
}

//--------------------------------------------
/// Get current uid
//--------------------------------------------
uid_t IoStat::getUid() const {return (_uid);}

//--------------------------------------------
/// Get current gid
//--------------------------------------------
gid_t IoStat::getGid() const {return (_gid);}

//--------------------------------------------
/// Get current app name
//--------------------------------------------
const std::string& IoStat::getApp() const {return (_app);}

//--------------------------------------------
/// Get the size of corresponding
/// READ or WRITE deck
//--------------------------------------------
ssize_t IoStat::getSize(Marks enumMark) const{
	if (enumMark == Marks::READ)
		return _readMarks.size();
	if (enumMark == Marks::WRITE)
		return _writeMarks.size();
	return 0;
}

//--------------------------------------------
/// Overload operator << 
//--------------------------------------------
std::ostream& operator<<(std::ostream &os, const IoStat &other){
	std::pair<double, double> read = other.bandWidth(IoStat::Marks::READ);
	std::pair<double, double> write = other.bandWidth(IoStat::Marks::WRITE);
	os << "[IoStat bandwidth from last 10s] " << std::endl;
	os << C_BLUE << "[READ]{average: " << read.first <<
		", standard deviation: " << read.second <<  "}";
	os << " / ";
	os <<  "[WRITE]{average: " << write.first <<
		", standard deviation: " << write.second <<  "}" << C_RESET << std::endl;
	return os;
}

//--------------------------------------------
/// Overload operator << to print a entire multimap
//--------------------------------------------
std::ostream& operator<<(std::ostream &os, const std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > &other){
	for (auto it : other){
		os << C_GREEN << "┌─[" << C_CYAN << "IoMap" << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "id:" << it.first << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" <<  C_CYAN << "app:"<< it.second->getApp() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "uid:" << it.second->getUid() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "gid:" << it.second->getGid() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "sR:" << it.second->getSize(IoStat::Marks::READ)
			<< "/sW:"<< it.second->getSize(IoStat::Marks::WRITE) << C_GREEN << "]" << C_RESET;
		os << std::endl << C_GREEN << "└─[" << C_CYAN << "IoStat" << C_GREEN << "]" << C_RESET;
		os << std::fixed << std::setprecision(3) << C_WHITE << *it.second << C_RESET << std::endl;
	}
	return os;
}


//--------------------------------------------
/// Overload operator << to print a IoStatSummary struct
//--------------------------------------------
std::ostream& operator<<(std::ostream &os, const std::optional<IoStatSummary> &opt){
	if (!opt.has_value()){
		os << "summary empty" << std::endl;
		return os;
	}
	IoStatSummary other = opt.value();
	os << "[READ]";
	if (other.readBandwidth.has_value())
		os << "[average: " << other.readBandwidth->first
			<< "][standard deviation: " << other.readBandwidth->second
			<< "][size: " << other.rSize << "] / ";
	else
		os << "[empty]" << std::endl;
	os << "[WRITE]";
	if (other.writeBandwidth.has_value())
		os << "[average: " << other.writeBandwidth->first
			<< "][standard deviation: " << other.writeBandwidth->second
			<< "][size: " << other.wSize << "]" << std::endl;
	else
		os << "[empty]" << std::endl;
	return os;
}

//--------------------------------------------
/// Overload operator << to print a IoStatSummary struct
//--------------------------------------------
std::ostream& operator<<(std::ostream &os, const IoStatSummary &other){
	os << "[READ]";
	if (other.readBandwidth.has_value())
		os << "[average: " << other.readBandwidth->first
			<< "][standard deviation: " << other.readBandwidth->second
			<< "][size: " << other.rSize << "] / ";
	else
		os << "[empty]" << std::endl;
	os << "[WRITE]";
	if (other.writeBandwidth.has_value())
		os << "[average: " << other.writeBandwidth->first
			<< "][standard deviation: " << other.writeBandwidth->second
			<< "][size: " << other.wSize << "]" << std::endl;
	else
		os << "[empty]" << std::endl;
	return os;
}
