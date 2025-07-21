#include <chrono>
#include <deque>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <mutex>
#include <optional>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <cstdint>
#include <sys/types.h>
#include <cmath>
#include <iostream>
#include <ctime>
#include <string.h>

#define IOSTAT_NAME "IoStat"

struct IoMark {
	struct timespec io_time;
    size_t bytes;

    IoMark(size_t bytes) : bytes(bytes){
		clock_gettime(CLOCK_REALTIME, &io_time);
	}

    IoMark() : bytes(0){
		clock_gettime(CLOCK_REALTIME, &io_time);
	}
};

// -------- IoStatSummary --------
struct IoStatSummary {
    double avg_read = 0.0;
    double sigma_read = 0.0;
    double avg_write = 0.0;
    double sigma_write = 0.0;
};




// -------- IoStat --------
class IoStat {
	// Enum
	public: enum class Marks{
		READ,
		WRITE
	};

	private:
		uint64_t _fileId;
		std::string _app;
		uid_t _uid;
		gid_t _gid;

		std::deque<IoMark> _readMarks;
		std::deque<IoMark> _writeMarks;


		IoStat();

	public:
		// Orthodoxe canonical form
		~IoStat();
		IoStat(const IoStat &other);
		IoStat& operator=(const IoStat &other);

		// default contructor
		IoStat(uint64_t fileId, const std::string& app, uid_t uid, gid_t gid);

		// Methode to add read and write bytes
		void addRead(size_t rBytes);
		void addWrite(size_t wBytes);

		// clean old mark oldest than <seconds>
		int cleanOldsMarks(Marks mark, size_t seconds = 10);

		// return the average and the standard derivation in the seconds range
		std::pair<double, double> bandWidth(Marks EnumMark, size_t seconds = 10) const;

		// PrintInfo
		static void	printInfo(std::ostream &os, const char *);
		static void	printInfo(std::ostream &os, const std::string &);

		// Getters
		uid_t getUid() const;
		gid_t getGid() const;
		const std::string& getApp() const;


};

// -------- IoMap --------
class IoMap {
public:
    IoMap();
    ~IoMap();

    void AddRead(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t rbytes);
    void AddWrite(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t wbytes);

    std::vector<std::string> getApps();
    std::vector<uid_t> getUids();
    std::vector<gid_t> getGids();

    std::optional<std::pair<double, double>> getBandwidth(const std::string& app, size_t past_seconds = 10);
    std::optional<std::pair<double, double>> getBandwidth(uid_t uid, size_t past_seconds = 10);

    std::unordered_multimap<uint64_t, std::shared_ptr<IoStat>> GetAllStatsSnapshot() const;

private:
    void CleanerLoop();
    void RemoveInactiveStats();

    mutable std::mutex mutex_;
    std::unordered_multimap<uint64_t, std::shared_ptr<IoStat>> file_map_;
    std::unordered_set<std::string> active_apps_;
    std::unordered_set<uid_t> active_uids_;
    std::unordered_set<gid_t> active_gids_;

    std::thread cleaner_thread_;
    std::atomic<bool> running_;
    std::condition_variable cv_;
};

// -------- IoAggregate --------
class IoAggregate {
public:
    IoAggregate(size_t num_bins, size_t interval_sec);

    void AddSample(const std::string& app, const IoStatSummary& summary);
    std::optional<IoStatSummary> GetAggregated(const std::string& app, size_t past_seconds) const;
    void ShiftWindow();

private:
    struct Bin {
        std::unordered_map<std::string, IoStatSummary> app_stats;
    };

    size_t num_bins_;
    size_t interval_sec_;
    size_t current_index_;
    std::vector<Bin> bins_;
    mutable std::mutex mutex_;
};

// -------- AggregatedIoMap --------
class AggregatedIoMap {
public:
    // Pass dynamic aggregation windows in seconds
    explicit AggregatedIoMap(const std::vector<size_t>& aggregation_windows);
    ~AggregatedIoMap();

    void AddRead(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t rbytes);
    void AddWrite(uint64_t inode, const std::string& app, uid_t uid, gid_t gid, size_t wbytes);

    std::optional<IoStatSummary> getBandwidth(const std::string& app, size_t past_seconds);
    std::optional<IoStatSummary> getBandwidth(uid_t uid, size_t past_seconds);

    std::vector<size_t> getAvailableWindows() const;

private:
    void AggregationLoop();
    size_t ComputeMaxIntervalSec() const;

    IoMap base_;
    std::unordered_map<size_t, std::unique_ptr<IoAggregate>> aggregates_;  // key: total window duration (sec)

    std::thread aggregation_thread_;
    std::atomic<bool> running_;
};



const char*	getCurrentTime();
