#include "bus.h"
#include "map.h"


void Bus::b_transport(int mid, tlm::tlm_generic_payload& tr, sc_time& delay)
{
	(void) mid;

	unsigned int addr = tr.get_address();
	int sid = decode_address(addr);
	tr.set_address(addr);

	this->initiators_socket[sid]->b_transport(tr, delay);

	addr = tr.get_address();
	encode_address(addr, sid);
	tr.set_address(addr);
}


unsigned int Bus::transport_dbg(int mid, tlm::tlm_generic_payload& tr)
{
	(void) mid;

	unsigned int addr = tr.get_address();
	int sid = decode_address(addr);
	tr.set_address(addr);

	unsigned int res = this->initiators_socket[sid]->transport_dbg(tr);

	addr = tr.get_address();
	encode_address(addr, sid);
	tr.set_address(addr);

	return res;
}


bool Bus::get_direct_mem_ptr(int mid, tlm::tlm_generic_payload& tr, tlm::tlm_dmi& dmi)
{
	(void) mid;

	unsigned int addr = tr.get_address();
	int sid = decode_address(addr);
	tr.set_address(addr);

	bool res = this->initiators_socket[sid]->get_direct_mem_ptr(tr, dmi);

	addr = tr.get_address();
	encode_address(addr, sid);
	tr.set_address(addr);

	return res;
}


void Bus::invalidate_direct_mem_ptr(int sid, sc_dt::uint64 start, sc_dt::uint64 end)
{
	encode_address((unsigned int&) start, sid);
	encode_address((unsigned int&) end, sid);
	for(unsigned int mid = 0; mid < this->targets_socket.size(); mid++)
		this->targets_socket[mid]->invalidate_direct_mem_ptr(start, end);
}
