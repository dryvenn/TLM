#ifndef __RAM_H__
#define __RAM_H__

#include <systemc.h>
#include <tlm.h>
#include "tlm_utils/simple_target_socket.h"

// Mandatory for simple sockets
#define SC_INCLUDE_DYNAMIC_PROCESSES
// RAM size in bytes
#define SIZE 256


SC_MODULE(RAM) {

	tlm_utils::simple_target_socket<RAM> socket;
	unsigned char data[SIZE];
	unsigned int size;

	SC_CTOR(RAM): socket("socket"), size(SIZE)
	{
		memset(data, 0xff, size);
		socket.register_b_transport(this, &RAM::b_transport);
		socket.register_transport_dbg(this, &RAM::transport_dbg);
	}

	virtual void b_transport(tlm::tlm_generic_payload&, sc_time&);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload&);
};

#endif
