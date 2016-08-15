#include <systemc.h>
#include "ram.h"
#include "cpu.h"


int sc_main (int argc, char * argv[])
{
	(void) argc; (void) argv;
	cout << "Hello, world!" << endl;

	CPU* cpu = new CPU("CPU");
	RAM* ram = new RAM("RAM");
	cpu->socket.bind(ram->socket);
	sc_start();

	return 0;
}
