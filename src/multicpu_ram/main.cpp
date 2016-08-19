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

	CPU* cpu0 = new CPU("CPU0", "./instructions0.asm", sc_time(2, SC_NS));
	CPU* cpu1 = new CPU("CPU1", "./instructions1.asm", sc_time(3, SC_NS));
	RAM* ram = new RAM("RAM");
	Bus* bus = new Bus("Bus");
	bus->initiator_socket.bind(ram->socket);
	cpu0->socket.bind(bus->targets_socket);
	cpu1->socket.bind(bus->targets_socket);
	sc_start();
	return 0;
}
