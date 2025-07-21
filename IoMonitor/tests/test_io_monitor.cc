#include "test.hh"

void fillIoStat(IoStat &io, int nbr = 10, int range = 100000){
	IoStat::printInfo(std::cout, "fill IoStat");

	for (int i = 0; i < nbr; i++)
		io.addRead(rand() % range);
	for (int i = 0; i < nbr; i++)
		io.addWrite(rand() % range);
}

void getBandWidth(IoStat &io, IoStat::Marks enumMark, int seconds = 10){
	std::cout << std::endl;
	IoStat::printInfo(std::cout, "Get bandWidth of " + std::to_string(seconds) + "s");
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

void sleeping(int seconds){
	std::string time = "sleeping " + std::to_string(seconds) + std::string("s...");
	IoStat::printInfo(std::cout, time);
	sleep(seconds);
}

void cleanMarks(IoStat &io, IoStat::Marks enumMark, int seconds){
	IoStat::printInfo(std::cout, "Cleaning everything after " + std::to_string(seconds) + "s");
	io.cleanOldsMarks(enumMark, seconds);
	std::cout << std::endl;
}

void testIoStat(){
	IoStat io(4, "mgm", 2, 2);

	fillIoStat(io);
	sleeping(2);
	fillIoStat(io);
	fillIoStat(io, 10000);
	sleeping(2);
	fillIoStat(io, 1000000000);
	// sleeping(1);
	fillIoStat(io, 10);
	getBandWidth(io, IoStat::Marks::READ, 100);
	cleanMarks(io, IoStat::Marks::READ, 2);
	getBandWidth(io, IoStat::Marks::READ, 100);
	getBandWidth(io, IoStat::Marks::READ, 3);

	sleeping(2);
	cleanMarks(io, IoStat::Marks::READ, 1);
	getBandWidth(io, IoStat::Marks::READ, 1000);
}
