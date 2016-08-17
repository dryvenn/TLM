#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>

// Mandatory for simple sockets
#define SC_INCLUDE_DYNAMIC_PROCESSES


SC_MODULE(Memory) {

	tlm_utils::simple_target_socket<Memory> socket;

	SC_CTOR(Memory): socket("socket")
	{
		socket.register_b_transport(this, &Memory::b_transport);
		socket.register_transport_dbg(this, &Memory::transport_dbg);
		socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
	}

	/* Target-related functions */
	virtual void b_transport(tlm::tlm_generic_payload&, sc_time&);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload&);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload&, tlm::tlm_dmi&);
	/* Memory-related functions, some to be implemented by children classes */
	virtual void dump_on_stdout(void);
	virtual unsigned int get_length(void) = 0;
	virtual unsigned char* get_memory_ptr(void) = 0;
	virtual int read(unsigned int addr, unsigned char* buff, unsigned int size) = 0;
	virtual int write(unsigned int addr, unsigned char* buff, unsigned int size) = 0;
	virtual int read_byte(unsigned int addr, unsigned char& byte) = 0;
	virtual int write_byte(unsigned int addr, unsigned char& byte) = 0;
};

#endif
