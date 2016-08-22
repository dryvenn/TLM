#include "ram.h"
#include "cpu.h"
#include "bus.h"
#include "arbitrator.h"


int sc_main (int argc, char * argv[])
{
	(void) argc; (void) argv;
	cout << "Hello, world!" << endl << endl;

	// Set the system global quantum
	tlm::tlm_global_quantum::instance().set(sc_time(10, SC_NS));

	CPU* cpu0 = new CPU("CPU0", "./instructions0.asm", sc_time(2, SC_NS));
	CPU* cpu1 = new CPU("CPU1", "./instructions1.asm", sc_time(1, SC_NS));
	RAM* ram = new RAM("RAM", sc_time(5, SC_NS));
	Arbitrator* arbitrator = new Arbitrator(2);
	Bus* bus = new Bus("Bus", arbitrator);
	bus->initiator_socket.bind(ram->socket);
	cpu0->socket.bind(bus->targets_socket);
	cpu1->socket.bind(bus->targets_socket);
	sc_start();
	return 0;
}
