#include "ioStat.hh"
#include <iomanip>

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
		friend std::ostream& operator<<(std::ostream &os, const std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator it);

		template <typename T>
		std::optional<std::pair<double, double>> getBandwidth(T index, IoStat::Marks enumMark, size_t seconds = 10){

		if (seconds == 0 || (enumMark != IoStat::Marks::READ && enumMark != IoStat::Marks::WRITE))
			return std::nullopt;

		std::vector<std::pair<double, double> > data = {{0, 0}};
		(void)data;
		(void)seconds;
		(void)index;

		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const std::string> || std::is_same_v<T, const char *>){
			// if (DEBUG == 2)
			// 	std::cout << this << std::endl;
			double average = 0;
			for (std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> >::iterator it = _filesMap.begin(); it != _filesMap.end(); it++){
				if (it->second->getApp() == index)
					average += it->second->bandWidth(IoStat::Marks::READ).first;
			}
			std::cout << average << std::endl;
		} else if constexpr (std::is_same_v<T, uid_t>){
		} else if constexpr (std::is_same_v<T, gid_t>){
		}
		else{
			std::cout << "???\n";
			return std::nullopt;
		}
		return data[0];
	}

};
