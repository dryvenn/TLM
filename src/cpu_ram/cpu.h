#ifndef __CPU_H__
#define __CPU_H__

#include <systemc.h>
#include <tlm.h>
#include "tlm_utils/simple_initiator_socket.h"

// Mandatory for simple sockets
#define SC_INCLUDE_DYNAMIC_PROCESSES


SC_MODULE(CPU) {

	tlm_utils::simple_initiator_socket<CPU> socket;
	tlm::tlm_dmi* dmi;

	SC_CTOR(CPU): socket("socket"), dmi(NULL)
	{
		SC_THREAD(test_ram);
		socket.register_invalidate_direct_mem_ptr(this, &CPU::invalidate_direct_mem_ptr);
	}

	void test_ram(void);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
};

#endif
