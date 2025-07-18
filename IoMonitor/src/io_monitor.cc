#include "../include/io_monitor.hh"

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

void IoStat::cleanOldsMarks(std::deque<IoMark> &mark, size_t seconds) const{
	(void)mark;
	(void)seconds;
}

std::pair<double, double> IoStat::ComputeStats(const std::deque<IoMark> &mark, size_t seconds) const{
	(void)mark;
	(void)seconds;
	return (std::pair(0, 0));
}

std::pair<double, double> IoStat::bandWidthRead(size_t seconds = 10) const{
	(void)seconds;
	return (std::pair(0, 0));
}

std::pair<double, double> IoStat::bandWidthWrite(size_t seconds = 10) const{
	(void)seconds;
	return (std::pair(0, 0));

}


uid_t IoStat::getUid() const {return (_uid);}

gid_t IoStat::getGid() const {return (_gid);}

const std::string& IoStat::getApp() const {return (_app);}
