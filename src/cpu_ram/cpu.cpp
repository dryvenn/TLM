#include "cpu.h"


void CPU::test_ram(void) {
	const unsigned int test = 0xdeadbeef;
	unsigned int data = test;
	const unsigned int byte_en = 0xffffff00;
	tlm::tlm_generic_payload* tr = new tlm::tlm_generic_payload();
	tr->set_command(tlm::TLM_WRITE_COMMAND);
	tr->set_address(0x00);
	tr->set_data_ptr((unsigned char*) &data);
	tr->set_data_length(sizeof(test));
	tr->set_byte_enable_ptr((unsigned char*) &byte_en);
	tr->set_byte_enable_length(sizeof(byte_en));
	tr->set_streaming_width(sizeof(test));
	tr->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = SC_ZERO_TIME;

	CPU::socket->b_transport(*tr, delay);

	if(tr->is_response_error()) {
		cerr << "Error: " + tr->get_response_string() << endl;
		return;
	}

	tr->set_command(tlm::TLM_READ_COMMAND);

	unsigned int read_bytes = CPU::socket->transport_dbg(*tr);

	if(read_bytes != sizeof(test)) {
		cerr << "Error: transport_dbg failed, read " << read_bytes << " bytes" << endl;
		return;
	}
	if(data != (test & byte_en)) {
		cerr << "Error: response doesn't match" << endl;
		return;
	}

	cout << "Test RAM OK." << endl;
};

void CPU::invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end)
{
	(void) start; (void) end;

	if(CPU::dmi != NULL)
		delete CPU::dmi;
}
