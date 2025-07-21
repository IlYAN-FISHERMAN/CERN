#include "test.hh"

void fillIoStat(IoStat &io, int nbr = 10, int range = 100000){
	// IoStat::printInfo(std::cout, "fill IoStat");

	for (int i = 0; i < nbr; i++)
		io.addRead(rand() % range);
	for (int i = 0; i < nbr; i++)
		io.addWrite(rand() % range);
}

void getBandWidth(IoStat &io, IoStat::Marks enumMark, int seconds = 10){
	std::cout << std::endl;
	IoStat::printInfo(std::cout, "Get bandwidth from the last " + std::to_string(seconds) + "s");
	if (enumMark != IoStat::Marks::READ && enumMark != IoStat::Marks::WRITE){
		return ;
	}

	std::pair<double, double> bandWidth = io.bandWidth(IoStat::Marks::READ, seconds);

	if (enumMark == IoStat::Marks::READ)
		std::cout << "\t[Read]: average: " << bandWidth.first << " | standard deviation: " << bandWidth.second << std::endl;
	else
		std::cout << "\t[Write]: average: " << bandWidth.first << " | standard deviation: " << bandWidth.second << std::endl;
	std::cout << std::endl;
}

void sleeping(double seconds){
	// std::string time = "sleeping " + std::to_string(seconds) + std::string("s...");
	// IoStat::printInfo(std::cout, time);
	sleep(seconds);
}

void cleanMarks(IoStat &io, IoStat::Marks enumMark, int seconds){
	IoStat::printInfo(std::cout, "Clean everything after " + std::to_string(seconds) + "s");
	io.cleanOldsMarks(enumMark, seconds);
	std::cout << std::endl;
}

int testIoStat(){
	IoStat io(4, "mgm", 2, 2);

	for (int i = 0; i < 800; i++){
		fillIoStat(io, 10000);
		sleeping(0.5);
	}
	getBandWidth(io, IoStat::Marks::READ, 100);
	cleanMarks(io, IoStat::Marks::READ, 1);
	getBandWidth(io, IoStat::Marks::READ, 100);
	sleeping(2);
	cleanMarks(io, IoStat::Marks::READ, 10);
	getBandWidth(io, IoStat::Marks::READ, 2);
	return 0;
}
