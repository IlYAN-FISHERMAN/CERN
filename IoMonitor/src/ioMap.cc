#include "../include/ioMap.hh"

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

void IoMap::cleanerLoop(){

	std::unique_lock<std::mutex> lock(_mutex);

	while (true){
		_cv.wait_for(lock, std::chrono::seconds(60));

		std::cout << "thread\n";
		if (!_running)
			break;

		std::cout << "thread cleaning!\n";
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
	
	for(;it.first != it.second; it.first++){
		auto &io = it.first->second;
		if (io->getApp() == app && io->getGid() == gid && io->getUid() == uid){
			io->addRead(rbytes);
			break ;
		}
	}
	if (it.first == it.second){
		auto newIo = _filesMap.insert({inode, std::make_shared<IoStat>(inode, app, uid, gid)});
		newIo->second->addRead(rbytes);
		_apps.insert(app);
		_uids.insert(uid);
		_gids.insert(gid);
	}
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
