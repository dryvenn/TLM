#ifndef __BUS_H__
#define __BUS_H__

#include <systemc.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include "arbitrator.h"


SC_MODULE(Bus) {
	// Multiple CPUs for one RAM
	tlm_utils::multi_passthrough_target_socket<Bus> targets_socket;
	tlm_utils::simple_initiator_socket<Bus> initiator_socket;
	Arbitrator* arbitrator;


	Bus(sc_module_name name, Arbitrator* arbitrator): sc_module(name), targets_socket("targets_socket"), initiator_socket("initiator_socket"), arbitrator(arbitrator) {

		targets_socket.register_b_transport(this, &Bus::b_transport);
		targets_socket.register_transport_dbg(this, &Bus::transport_dbg);
		targets_socket.register_get_direct_mem_ptr(this, &Bus::get_direct_mem_ptr);
		initiator_socket.register_invalidate_direct_mem_ptr(this, &Bus::invalidate_direct_mem_ptr);
	}


	virtual void b_transport(int, tlm::tlm_generic_payload&, sc_time&);
	virtual unsigned int transport_dbg(int, tlm::tlm_generic_payload&);
	virtual bool get_direct_mem_ptr(int, tlm::tlm_generic_payload&, tlm::tlm_dmi&);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
};


#endif
