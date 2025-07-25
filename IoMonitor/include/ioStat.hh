#include "ioMonitor.hh"

#define IOSTAT_NAME "IoStat"

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
		ssize_t getSize(Marks enumMark) const;

};

std::ostream& operator<<(std::ostream &os, const IoStat *other);

struct IoStatSummary {
    double avg_read = 0.0;
    double sigma_read = 0.0;
    double avg_write = 0.0;
    double sigma_write = 0.0;
};
