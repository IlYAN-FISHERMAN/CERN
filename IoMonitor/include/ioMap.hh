#include "ioStat.hh"

#define IOMAP_NAME "IoMap"

class IoMap {
	private:
		void cleanerLoop();
		void removeInactives();

		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > _filesMap;
		std::unordered_set<std::string> _apps;
		std::unordered_set<uid_t> _uids;
		std::unordered_set<gid_t> _gids;

		mutable std::mutex _mutex;
		std::thread _cleaner;
		std::atomic<bool> _running;
		std::condition_variable _cv;

	public:
		// Orthodoxe Canonical Form
		IoMap();
		IoMap(int);
		~IoMap();
		IoMap(const IoMap &other);
		IoMap& operator=(const IoMap &other);

		void AddRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes);
		void AddWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes);

		std::vector<std::string> getApps();
		std::vector<uid_t> getUids();
		std::vector<gid_t> getGids();
		

		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat>> GetAllStatsSnapshot() const;

		template <typename T>
		std::optional<std::pair<double, double>> getBandwidth(T index, IoStat::Marks enumMark, size_t seconds = 10){

		if (seconds == 0 || (enumMark != IoStat::Marks::READ && enumMark != IoStat::Marks::WRITE))
			return std::nullopt;

		std::pair<double, double> data = {0, 0};
		(void)index;
		(void)seconds;

		if constexpr (std::is_same_v<T, uint64_t>){
			// auto &it = _filesMap.equal_range(index);
			// if (it.first == it.second)
			// 	return std::nullopt;
			// size_t size = std::distance(it.first, it.second);
			// for (;it.first != it.second; it.first++){
			// 	data += it.first->second->bandWidth(enumMark, seconds);
			// }
			// data.first /= size;
		} else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>){
		} else if constexpr (std::is_same_v<T, uid_t>){
		} else if constexpr (std::is_same_v<T, gid_t>){
		}
		else
			return std::nullopt;
		return data;
	}
};
