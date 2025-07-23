#include "ioStat.hh"

#define IOMAP_NAME "IoMap"

class IoMap {
	private:
		void cleanerLoop();
		void removeInactives();

		mutable std::mutex _mutex;
		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat> > _filesMap;
		std::unordered_set<std::string> _apps;
		std::unordered_set<uid_t> _uids;
		std::unordered_set<gid_t> _gids;

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

		void AddRead(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t rbytes);
		void AddWrite(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t wbytes);

		std::vector<std::string> getApps();
		std::vector<uid_t> getUids();
		std::vector<gid_t> getGids();

		std::optional<std::pair<double, double>> getBandwidth(const std::string& app, size_t past_seconds = 10);
		std::optional<std::pair<double, double>> getBandwidth(uid_t uid, size_t past_seconds = 10);

		std::unordered_multimap<uint64_t, std::shared_ptr<IoStat>> GetAllStatsSnapshot() const;

		void test();
};
