#include <systemc.h>
#include "ram.h"
#include "initiator.h"


int sc_main (int argc, char * argv[])
{
	(void) argc; (void) argv;
	cout << "Hello, world!" << endl;

	// Set the system global quantum
	tlm::tlm_global_quantum::instance().set(sc_time(10, SC_NS));

	Initiator* initiator = new Initiator("Initiator");
	RAM* ram = new RAM();
	initiator->socket.bind(ram->socket);
	sc_start();

	return 0;
}
