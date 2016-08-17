#ifndef __INITIATOR_H__
#define __INITIATOR_H__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/tlm_quantumkeeper.h>

// Mandatory for simple sockets
#define SC_INCLUDE_DYNAMIC_PROCESSES
// The fake assembly file
#define INSTRUCTION_FILE "./instructions.a"


SC_MODULE(Initiator) {

	tlm_utils::simple_initiator_socket<Initiator> socket;
	tlm::tlm_dmi* dmi;
	tlm_utils::tlm_quantumkeeper q_keeper;

	SC_CTOR(Initiator): socket("socket"), dmi(NULL)
	{
		/* Set the thread's global quantum to the system global quantum.
		 * Reset the local time offset.
		 */
		const sc_time global_q = tlm::tlm_global_quantum::instance().get();
		q_keeper.set_global_quantum(global_q);
		q_keeper.reset();

		socket.register_invalidate_direct_mem_ptr(this, &Initiator::invalidate_direct_mem_ptr);

		SC_THREAD(test_ram);
	}

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
	void test_ram(void);
	void execute_instruction_file(void);
};

#endif
