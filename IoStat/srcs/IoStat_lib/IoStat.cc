#include "IoStat.hh"

IoStat::IoStat(){}

IoStat::~IoStat(){}

IoStat::IoStat(const IoStat &other){
	(void)other;
}

IoStat& IoStat::operator=(const IoStat &other){
	if (this != &other){
		;
	}
	return (*this);
}
