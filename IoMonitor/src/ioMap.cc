#include "../include/ioMap.hh"

std::mutex IoMap::_osMutex;

//--------------------------------------------
/// Main constructor
//--------------------------------------------
IoMap::IoMap() : _running(true){
	_cleaner = std::thread(&IoMap::cleanerLoop, this);
}

//--------------------------------------------
/// Destructor
//--------------------------------------------
IoMap::~IoMap() {
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_running = false;
		_cv.notify_one();
	}
	if (_cleaner.joinable()){
		_cleaner.join();
	}
}

//--------------------------------------------
/// Constructor for disable mutlithreading
//--------------------------------------------
IoMap::IoMap(int) : _running(false){}

//--------------------------------------------
/// Display the string given as parameter in
/// specific format with the current time
//--------------------------------------------
void	IoMap::printInfo(std::ostream &os, const char *msg){
	const char *time = getCurrentTime();
	os << IOMAP_NAME << " [" << time << "]: " << msg << std::endl;
}

//--------------------------------------------
/// Display the string given as parameter in
/// specific format with the current time
//--------------------------------------------
void	IoMap::printInfo(std::ostream &os, const std::string &msg){
	const char *time = getCurrentTime();
	os << IOMAP_NAME << " [" << time << "]: " << msg << std::endl;
}

//--------------------------------------------
/// Multithreaded function to clean up inactive IoStats
//--------------------------------------------
void IoMap::cleanerLoop(){
	while (true){
		std::unique_lock<std::mutex> lock(_mutex);
		_cv.wait_for(lock, std::chrono::seconds(TIME_TO_CLEAN), [this]{ return !_running;});
		if (!_running)
			break;
		// Clean inactive I/O
		size_t rsize = 0;
		size_t wsize = 0;
		for(auto it = _filesMap.begin(); it != _filesMap.end();){
			std::pair<double, double> read;
			std::pair<double, double> write;
			read = it->second->bandWidth(IoStat::Marks::READ, &rsize, TIME_TO_CLEAN);
			write = it->second->bandWidth(IoStat::Marks::WRITE, &wsize, TIME_TO_CLEAN);
			if ((read.first == 0 && read.second == 0)
				&& (write.first == 0 && write.second == 0)
				&& (rsize == 0 && wsize == 0))
				it = _filesMap.erase(it);
			else
				it++;
			rsize = 0;
			wsize = 0;
		}
	}
}

//--------------------------------------------
/// Adds an IoStat object to the multimap with the corresponding elements
//--------------------------------------------
void IoMap::addRead(uint64_t inode, std::string app, uid_t uid, gid_t gid, size_t rbytes){
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _filesMap.equal_range(inode);
	// Create new IoStat or search for an existing matching one
	if (_filesMap.find(inode) == _filesMap.end()){
		if (io::IoMapDebug)
			printInfo(std::cout, "add new");
		auto newIo = _filesMap.insert({inode, std::make_shared<IoStat>(inode, app, uid, gid)});
		newIo->second->add(rbytes, IoStat::Marks::READ);
		_apps.insert(app);
		_uids.insert(uid);
		_gids.insert(gid);
		return ;
	}
	while (it.first != it.second){
		auto &io = it.first->second;
		if (io->getApp() == app && io->getGid() == gid && io->getUid() == uid){
			io->add(rbytes, IoStat::Marks::READ);
			if (io::IoMapDebug)
				printInfo(std::cout, "addRead");
			break ;
		}
		it.first++;
		if (it.first == it.second){
			if (io::IoMapDebug)
				printInfo(std::cout, "add new");
			auto newIo = _filesMap.insert({inode, std::make_shared<IoStat>(inode, app, uid, gid)});
			newIo->second->add(rbytes, IoStat::Marks::READ);
			_apps.insert(app);
			_uids.insert(uid);
			_gids.insert(gid);
			break ;
		}
	}
}

//--------------------------------------------
/// Adds an IoStat object to the multimap with the corresponding elements
//--------------------------------------------
void IoMap::addWrite(uint64_t inode, std::string app, uid_t uid, gid_t gid, size_t wbytes){
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _filesMap.equal_range(inode);
	// Create new IoStat or search for an existing matching one
	if (_filesMap.find(inode) == _filesMap.end()){
		if (io::IoMapDebug)
			printInfo(std::cout, "add new");
		auto newIo = _filesMap.insert({inode, std::make_shared<IoStat>(inode, app, uid, gid)});
		newIo->second->add(wbytes, IoStat::Marks::WRITE);
		_apps.insert(app);
		_uids.insert(uid);
		_gids.insert(gid);
		return ;
	}
	while (it.first != it.second){
		auto &io = it.first->second;
		if (io->getApp() == app && io->getGid() == gid && io->getUid() == uid){
			io->add(wbytes, IoStat::Marks::WRITE);
			if (io::IoMapDebug)
				printInfo(std::cout, "addWrite");
			break ;
		}
		it.first++;
		if (it.first == it.second){
			if (io::IoMapDebug)
				printInfo(std::cout, "add new");
			auto newIo = _filesMap.insert({inode, std::make_shared<IoStat>(inode, app, uid, gid)});
			newIo->second->add(wbytes, IoStat::Marks::WRITE);
			_apps.insert(app);
			_uids.insert(uid);
			_gids.insert(gid);
			break ;
		}
	}
}


//--------------------------------------------
/// Get all apps
//--------------------------------------------
std::vector<std::string> IoMap::getApps() const{
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector<std::string> appsName(_apps.begin(), _apps.end());
	return (appsName);
}

//--------------------------------------------
/// Get all uids
//--------------------------------------------
std::vector<uid_t> IoMap::getUids() const{
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector<uid_t> uids(_uids.begin(), _uids.end());
	return (uids);
}

//--------------------------------------------
/// Get all gids
//--------------------------------------------
std::vector<gid_t> IoMap::getGids() const{
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector<gid_t> gids(_gids.begin(), _gids.end());
	return (gids);
}

//--------------------------------------------
/// Get a copy of the multimap
//--------------------------------------------
std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > IoMap::GetAllStatsSnapshot() const{
	std::lock_guard<std::mutex> lock(_mutex);
	return (_filesMap);
}

//--------------------------------------------
/// Overload operator << to print the entire
/// multimap from a IoMap object
//--------------------------------------------
std::ostream& operator<<(std::ostream &os, const IoMap &other){
	for (auto it : other._filesMap){
		os << C_GREEN << "┌─[" << C_CYAN << "IoMap" << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "id:" << it.first << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" <<  C_CYAN << "app:"<< it.second->getApp() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "uid:" << it.second->getUid() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "gid:" << it.second->getGid() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "sR:" << it.second->getSize(IoStat::Marks::READ)
			<< "/sW:"<< it.second->getSize(IoStat::Marks::WRITE) << C_GREEN << "]" << C_RESET;
		os << std::endl << C_GREEN << "└─[" << C_CYAN << "IoStat" << C_GREEN << "]" << C_RESET;
		os << std::fixed << std::setprecision(3) << C_WHITE << it.second << C_RESET << std::endl;
	}
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
		os << std::fixed << std::setprecision(3) << C_WHITE << it.second << C_RESET << std::endl;
	}
	return os;
}
