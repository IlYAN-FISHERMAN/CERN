#include "ioMap.hh"

#define IOAGGREGATE_NAME "IoAggregate"

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

