#include <systemc.h>
#include "ram.h"
#include "cpu.h"
#include "bus.h"


int sc_main (int argc, char * argv[])
{
	(void) argc; (void) argv;
	cout << "Hello, world!" << endl << endl;

	// Set the system global quantum
	tlm::tlm_global_quantum::instance().set(sc_time(10, SC_NS));

	CPU* cpu = new CPU("CPU", "./instructions.asm");
	RAM* ram = new RAM("RAM");
	Bus* bus = new Bus("Bus");
	bus->initiator_socket.bind(ram->socket);
	cpu->socket.bind(bus->targets_socket);
	sc_start();
	return 0;
}
