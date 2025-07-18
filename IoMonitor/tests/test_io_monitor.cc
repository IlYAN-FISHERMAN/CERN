#include "../include/io_monitor.hh"

void testIoStat(){
	int fileId = 4;

	IoStat io(fileId, "mgm", 2, 2);
	for (int i = 0, ran = rand() % 100; i < ran; i++)
		io.addRead(ran * rand() % 100000);
	for (int i = 0, ran = rand() % 100; i < ran; i++)
		io.addWrite(ran * rand() % 100000);
	io.bandWidthRead();
}
