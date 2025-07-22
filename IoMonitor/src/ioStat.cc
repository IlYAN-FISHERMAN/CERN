#include "../include/ioStat.hh"

const char* getCurrentTime(){
	time_t timestamp;
	std::time(&timestamp);
	char *time = std::ctime(&timestamp);
	if (!time)
		exit (EXIT_FAILURE);
	time[strlen(time) - 1] = 0;
	return (time);
}

IoStat::~IoStat(){}

IoStat::IoStat(const IoStat &other) :
	_fileId(other._fileId), _app(other._app), _uid(other._uid),
	_gid(other._gid), _readMarks(other._readMarks), _writeMarks(other._writeMarks){
}

IoStat& IoStat::operator=(const IoStat &other){
	if (this != &other){
		_fileId = other._fileId;
		_app = other._app;
		_uid = other._uid;
		_gid = other._gid;
		_readMarks = other._readMarks;
		_writeMarks = other._writeMarks;
	}
	return (*this);
}
void	IoStat::printInfo(std::ostream &os, const char *msg){
	const char *time = getCurrentTime();
	os << IOSTAT_NAME << " [" << time << "]: " << msg << std::endl;
}

void	IoStat::printInfo(std::ostream &os, const std::string &msg){
	const char *time = getCurrentTime();
	os << IOSTAT_NAME << " [" << time << "]: " << msg << std::endl;
}

IoStat::IoStat(uint64_t fileId, const std::string& app, uid_t uid, gid_t gid) :
	_fileId(fileId), _app(app), _uid(uid), _gid(gid){}

void IoStat::addRead(size_t rBytes){
	IoMark io(rBytes);
	_readMarks.push_back(io);
}

void IoStat::addWrite(size_t wBytes){
		IoMark io(wBytes);
	_writeMarks.push_back(io);
}

int IoStat::cleanOldsMarks(Marks enumMark, size_t seconds){
	if ((enumMark != Marks::READ && enumMark != Marks::WRITE) || seconds == 0){
		if (DEBUG)
			IoStat::printInfo(std::cerr, "\033[031mNo marks found for\033[0m");
		return -1;
	}

	std::deque<IoMark> &mark = (enumMark == Marks::READ ? _readMarks : _writeMarks);

	std::deque<IoMark>::const_iterator begin = mark.begin();
	std::deque<IoMark>::const_iterator end = mark.end();
	struct timespec	currentTime;

	clock_gettime(CLOCK_REALTIME, &currentTime);
	for (std::deque<IoMark>::const_iterator it = begin; it != mark.end(); it++){
		if ((difftime(currentTime.tv_sec, it->io_time.tv_sec) < seconds))
			break;
		end = it;
	}
	if (end == mark.end()){
		if (DEBUG)
			printInfo(std::cout, "\033[031mNothing to clean\033[0m");
		return 1;
	}

	if (DEBUG){
		std::cout << "Clean Range(" << std::distance(begin, end) << "/" << mark.size() << "):" << std::endl;
		std::cout << "{ \n";
		for (auto it = begin; it != end; it++)
			std::cout << "\t" << it->bytes << std::endl;
		std::cout << "}\n" << std::endl;	
	}
	
	size_t size = std::distance(begin, end);
	mark.erase(begin, end);

	return size;
}

std::pair<double, double> IoStat::bandWidth(Marks enumMark, size_t seconds) const{
	if ((enumMark != Marks::READ && enumMark != Marks::WRITE) || seconds == 0){
		if (DEBUG)
			IoStat::printInfo(std::cerr, "\033[031mNo marks found for\033[0m");
		return (std::pair(0, 0));
	}

	const std::deque<IoMark> &mark = (enumMark == Marks::READ ? _readMarks : _writeMarks);
	double avrg = 0;
	double deviation = 0;
	std::deque<IoMark>::const_iterator begin = mark.end();
	std::deque<IoMark>::const_iterator end = mark.end();
	struct timespec	currentTime;

	// Get the range of Marks
	clock_gettime(CLOCK_REALTIME, &currentTime);
	for (std::deque<IoMark>::const_iterator it = end; it != mark.begin();){
		it--;
		if ((difftime(currentTime.tv_sec, it->io_time.tv_sec) > seconds))
			break;
		begin = it;
	}
	if (DEBUG){
		std::cout << "Range(" << std::distance(begin, end) << "/" << mark.size() << "):" << std::endl;
		std::cout << "{ \n";
		for (auto it = begin; it != end; it++)
			std::cout << "\t" << it->bytes << std::endl;
		std::cout << "}\n" << std::endl;
	}

	if (begin == end)
		return (std::pair(0, 0));

	// Calcule average and standard deviation
	for (std::deque<IoMark>::const_iterator it = begin; it < end; it++)
		avrg += static_cast<double>(it->bytes);
	avrg = avrg / std::distance(begin, end);

	for (std::deque<IoMark>::const_iterator it = begin; it < end; it++){
		double nbr = static_cast<double>(it->bytes - avrg);
		deviation += (nbr * nbr);
	}
	deviation = sqrt(deviation / (std::distance(begin, end)));

	return (std::pair<double, double>(avrg, deviation));
}

uid_t IoStat::getUid() const {return (_uid);}

gid_t IoStat::getGid() const {return (_gid);}

const std::string& IoStat::getApp() const {return (_app);}

ssize_t IoStat::getSize(Marks enumMark) const{
	if (enumMark == Marks::READ)
		return _readMarks.size();
	if (enumMark == Marks::WRITE)
		return _writeMarks.size();
	return 0;
}
