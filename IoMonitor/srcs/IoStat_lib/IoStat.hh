#include <iostream>
#include <chrono>

class IoStat{
	private:

	public:
	// Orthodoxe Canonical form
	IoStat();
	~IoStat();
	IoStat(const IoStat &);
	IoStat& operator=(const IoStat&);

	IoStat(uint64_t fileid, const std::string& app, uid_t uid, gid_t gid);
	void addRead(size_t rbytes);
	void addWrite(size_t wbytes);
	double bandWidthRead(size_t pastseconds=10) const;
	double bandWidhtWrite(size_t pastsecond=10) const;
};


struct IoMark{
    struct timespec io_time; // unixtime with ns resolution when IO started
    size_t bytes; // transferd
};
