#ifndef __MASTER_H__
#define __MASTER_H__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/tlm_quantumkeeper.h>

// Mandatory for simple sockets
#define SC_INCLUDE_DYNAMIC_PROCESSES


SC_MODULE(Master) {

	tlm_utils::simple_initiator_socket<Master> socket;
	tlm::tlm_dmi* dmi;
	tlm_utils::tlm_quantumkeeper q_keeper;
	tlm::tlm_generic_payload* payload;

	SC_CTOR(Master): socket("socket"), dmi(NULL) {
		/* Set the thread's global quantum to the system global quantum.
		 * Reset the local time offset.
		 */
		const sc_time global_q = tlm::tlm_global_quantum::instance().get();
		q_keeper.set_global_quantum(global_q);
		q_keeper.reset();

		payload = new tlm::tlm_generic_payload();

		socket.register_invalidate_direct_mem_ptr(this, &Master::invalidate_direct_mem_ptr);

		SC_THREAD(process);
	}

	~Master() {
		this->invalidate_direct_mem_ptr(0, 0);
		delete this->payload;
	}

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
	virtual int read(unsigned int addr, unsigned char* buff, unsigned int size);
	virtual int write(unsigned int addr, unsigned char* buff, unsigned int size);
	virtual int read_byte(unsigned int addr, unsigned char& byte);
	virtual int write_byte(unsigned int addr, unsigned char& byte);
	virtual void sleep(sc_time duration);
	// Tests the ram when executed with SC_THREAD.
	virtual void test_ram(void);
	// Threaded process to be implemented by the children.
	virtual void process(void) = 0;
};

#endif
