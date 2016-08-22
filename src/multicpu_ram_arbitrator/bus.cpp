#include "bus.h"


void Bus::b_transport(int id, tlm::tlm_generic_payload& tr, sc_time& delay)
{
	this->arbitrator->process_transaction(id, tr);
	if(tr.get_response_status() != tlm::TLM_INCOMPLETE_RESPONSE)
		return;
	this->initiator_socket->b_transport(tr, delay);
}


unsigned int Bus::transport_dbg(int id, tlm::tlm_generic_payload& tr)
{
	(void) id;
	return this->initiator_socket->transport_dbg(tr);
}


bool Bus::get_direct_mem_ptr(int id, tlm::tlm_generic_payload& tr, tlm::tlm_dmi& dmi)
{
	(void) id;
	return this->initiator_socket->get_direct_mem_ptr(tr, dmi);
}


void Bus::invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end)
{
	for(unsigned int i = 0; i < this->targets_socket.size(); i++)
		this->targets_socket[i]->invalidate_direct_mem_ptr(start, end);
}
