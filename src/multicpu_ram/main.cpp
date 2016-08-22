#include <systemc.h>
#include "ram.h"
#include "cpu.h"
#include "bus.h"
#include "gpio.h"


int sc_main (int argc, char * argv[])
{
	(void) argc; (void) argv;
	cout << "Hello, world!" << endl << endl;

	// Set the system global quantum
	tlm::tlm_global_quantum::instance().set(sc_time(10, SC_NS));

	CPU* cpu0 = new CPU("CPU0", "./instructions0.asm", sc_time(2, SC_NS));
	CPU* cpu1 = new CPU("CPU1", "./instructions1.asm", sc_time(1, SC_NS));
	RAM* ram = new RAM("RAM", sc_time(5, SC_NS));
	GPIO* gpio = new GPIO("GPIO", sc_time(1, SC_NS), sc_time(2, SC_NS));
	Bus* bus = new Bus("Bus");
	// RAM_ID == 0 (see map.h)
	bus->initiators_socket.bind(ram->socket);
	// GPIO_ID == 1 (see map.h)
	bus->initiators_socket.bind(gpio->socket);
	cpu0->socket.bind(bus->targets_socket);
	cpu1->socket.bind(bus->targets_socket);
	sc_start();
	return 0;
}
