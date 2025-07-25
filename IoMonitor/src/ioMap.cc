#include "../include/ioMap.hh"

std::ostream& operator<<(std::ostream &os, const IoMap *other){
	for (auto it : other->_filesMap){
		os << C_GREEN << "┌─[" << C_CYAN << "IoMap" << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "id:" << C_CYAN << it.first << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" <<  C_CYAN << "app:" << C_CYAN << it.second->getApp() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "uid:" << C_CYAN << it.second->getUid() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "gid:" << C_CYAN << it.second->getGid() << C_GREEN << "]" << C_RESET;
		os << C_GREEN << "[" << C_CYAN << "sR:" << C_CYAN << it.second->getSize(IoStat::Marks::READ) << C_GREEN
			<< "/" << C_CYAN << "sW:"<< C_WHITE << it.second->getSize(IoStat::Marks::WRITE) << C_GREEN << "]" << C_RESET;
		os << std::endl << C_GREEN << "└─[IoStat]" << C_RESET;
		os << std::fixed << std::setprecision(3) << C_WHITE << it.second << C_RESET << std::endl;
	}
	return os;
}

IoMap::IoMap() : _running(true){
	_cleaner = std::thread(&IoMap::cleanerLoop, this);
}

IoMap::~IoMap() {
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_running = false;
	}
	if (_cleaner.joinable())
		_cleaner.join();
}

IoMap::IoMap(int) : _running(true){}

IoMap::IoMap(const IoMap &other){
	std::lock_guard<std::mutex> lock(other._mutex);
	_filesMap = other._filesMap;
	_apps = other._apps;
	_uids = other._uids;
	_gids = other._gids;
	_cleaner = std::thread(&IoMap::cleanerLoop, this);
}

IoMap& IoMap::operator=(const IoMap& other){
	if (this != &other){
		std::lock_guard<std::mutex> lock(other._mutex);
		_filesMap = other._filesMap;
		_apps = other._apps;
		_uids = other._uids;
		_gids = other._gids;
		_cleaner = std::thread(&IoMap::cleanerLoop, this);
	}
	return *this;
}

void	IoMap::printInfo(std::ostream &os, const char *msg){
	const char *time = getCurrentTime();
	os << IOMAP_NAME << " [" << time << "]: " << msg << std::endl;
}

void	IoMap::printInfo(std::ostream &os, const std::string &msg){
	const char *time = getCurrentTime();
	os << IOMAP_NAME << " [" << time << "]: " << msg << std::endl;
}

void IoMap::cleanerLoop(){

	std::unique_lock<std::mutex> lock(_mutex);

	while (true){
		_cv.wait_for(lock, std::chrono::seconds(60));

		printInfo(std::cout, "thread!");
		if (!_running)
			break;

		printInfo(std::cout, "thread cleaning!");
		for(auto it = _filesMap.begin(); it != _filesMap.end();){
			std::pair<double, double> read;
			std::pair<double, double> write;
			read = it->second->bandWidth(IoStat::Marks::READ, 60);
			write = it->second->bandWidth(IoStat::Marks::WRITE, 60);
			if ((read.first == 0 && read.second == 0) && (write.first == 0 && write.second == 0))
				it = _filesMap.erase(it);
			else
				it++;
		}
	}
}

void IoMap::AddRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes){
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _filesMap.equal_range(inode);
	if (DEBUG == 3){
		std::string data(app + " " + std::to_string(uid) + " " + std::to_string(gid) + " " + std::to_string(rbytes));
		printInfo(std::cout, "enter with: " + data);
	}
	for(;it.first != it.second; it.first++){
		auto &io = it.first->second;
		if (io->getApp() == app && io->getGid() == gid && io->getUid() == uid){
			io->addRead(rbytes);
			if (DEBUG == 3)
				printInfo(std::cout, "addRead");
			break ;
		}
	}
	if (it.first == it.second){
		if (DEBUG == 3)
			printInfo(std::cout, "add new");
		auto newIo = _filesMap.insert({inode, std::make_shared<IoStat>(inode, app, uid, gid)});
		newIo->second->addRead(rbytes);
		_apps.insert(app);
		_uids.insert(uid);
		_gids.insert(gid);
	}
	if (DEBUG == 3)
		printInfo(std::cout, "end\n");
}

void IoMap::AddWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes){
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _filesMap.equal_range(inode);
	
	for(;it.first != it.second; it.first++){
		auto &io = it.first->second;
		if (io->getApp() == app && io->getGid() == gid && io->getUid() == uid){
			io->addWrite(wbytes);
			break ;
		}
	}
	if (it.first == it.second){
		auto &&newIo = _filesMap.insert({inode, std::make_shared<IoStat>(inode, app, uid, gid)});
		newIo->second->addWrite(wbytes);
		_apps.insert(app);
		_uids.insert(uid);
		_gids.insert(gid);
	}
}

std::vector<std::string> IoMap::getApps(){
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector<std::string> appsName(_apps.begin(), _apps.end());
	return (appsName);
}

std::vector<uid_t> IoMap::getUids(){
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector<uid_t> uids(_uids.begin(), _uids.end());
	return (uids);
}

std::vector<gid_t> IoMap::getGids(){
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector<gid_t> gids(_gids.begin(), _gids.end());
	return (gids);
}

std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > IoMap::GetAllStatsSnapshot() const{
	std::lock_guard<std::mutex> lock(_mutex);
	return (_filesMap);
}
