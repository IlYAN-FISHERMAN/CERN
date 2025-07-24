#include "../include/ioMap.hh"

IoMap::IoMap() : _running(true){
	_cleaner = std::thread(&IoMap::cleanerLoop, this);
}

IoMap::~IoMap() {
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_running = false;
	}
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
		_cv.wait_for(lock, std::chrono::seconds(2), [&]{return !_running || !_filesMap.empty();});

		if (!_running)
			break;

		std::cout << "thread cleaning!\n";
		for(auto &it : _filesMap){
			it.second->cleanOldsMarks(IoStat::Marks::READ, 5);
			it.second->cleanOldsMarks(IoStat::Marks::WRITE, 5);
		}
	}
}

void IoMap::AddRead(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t rbytes){
}

void IoMap::AddWrite(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t wbytes){
}

std::optional<std::pair<double, double>> getBandwidth(const std::string& app, size_t past_seconds = 10){}

std::optional<std::pair<double, double>> getBandwidth(uid_t uid, size_t past_seconds = 10){}



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
