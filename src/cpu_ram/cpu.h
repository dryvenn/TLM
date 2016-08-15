#ifndef __CPU_H__
#define __CPU_H__

#include <systemc.h>
#include <tlm.h>
#include "tlm_utils/simple_initiator_socket.h"

// Mandatory for simple sockets
#define SC_INCLUDE_DYNAMIC_PROCESSES


SC_MODULE(CPU) {

	tlm_utils::simple_initiator_socket<CPU> socket;

	SC_CTOR(CPU): socket("socket")
	{
		SC_THREAD(test_ram);
	}

	void test_ram(void);
};

#endif
