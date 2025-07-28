#include "ioStat.hh"
#include <iomanip>
#include <numeric>

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

		static void	printInfo(std::ostream &os, const char *);
		static void	printInfo(std::ostream &os, const std::string &);

		friend std::ostream& operator<<(std::ostream &os, const IoMap *other);




		template <typename T>
		std::optional<std::pair<double, double>> getBandwidth(const T index, IoStat::Marks enumMark, size_t seconds = 10){

		if (seconds == 0 || (enumMark != IoStat::Marks::READ && enumMark != IoStat::Marks::WRITE))
			return std::nullopt;

		std::map<std::pair<double, double>, size_t> indexData;
		std::pair<double, double> weighted = {0, 0};
		std::pair<double, double> tmp = {0, 0};
		size_t size = 0;

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
			if (DEBUG == 2)
				printInfo(std::cerr, "No match found for data type");
			return std::nullopt;
		}
		if (indexData.size() <= 0)
			return std::nullopt;

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
