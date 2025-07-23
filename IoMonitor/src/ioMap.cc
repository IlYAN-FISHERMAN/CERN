#include "../include/ioMap.hh"

IoMap::IoMap() : _cleaner(std::thread{&IoMap::cleanerLoop, this}) , _running(true){
}

IoMap::~IoMap() {
	if (_running){
		_running = false;
		_cleaner.join();
	}
}

IoMap::IoMap(int){}

IoMap::IoMap(const IoMap& other){(void)other;}

IoMap& IoMap::operator=(const IoMap& other){
	(void)other;
	return *this;
}

// void AddRead(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t rbytes){}
//
// void AddWrite(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t wbytes){}


void IoMap::cleanerLoop(){
	while (_running){
		_mutex.lock();
		std::cout << "thread cleaning!\n";
		for(auto &it : _filesMap){
			if (!_filesMap.size())
				break;
			it.second->cleanOldsMarks(IoStat::Marks::READ, 5);
			it.second->cleanOldsMarks(IoStat::Marks::WRITE, 5);
		}
		_mutex.unlock();
		sleep(3);
	}
}

void IoMap::test(){
	std::unordered_multimap<int, std::string> map;
	// std::unordered_multiset<int> set = {1, 2, 3 ,4 ,5, 0, 10, 9 , 3414, 42};
	// set.insert(1);
	//
	// for (const auto &it : set){
	// 	if (it == 1)
	// 		std::cout << it << std::endl;
	// }
	for (const auto &it : map){
		std::cout << it.first << " " << it.second << std::endl;
	}
	// auto it = map.begin();
	// std::cout << map.size() << std::endl;
	// std::advance(it, 3);
	// std::cout << it->first << " " << it->second << std::endl;
}
