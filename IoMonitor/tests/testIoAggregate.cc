#include "tester.hh"

int testIoAggregateData(){
	std::vector<size_t> aggregateWin;
	
	aggregateWin.push_back(60);
	aggregateWin.push_back(3600);
	aggregateWin.push_back(300);
	IoAggregateMap agrMap(aggregateWin);

	return 0;
}
