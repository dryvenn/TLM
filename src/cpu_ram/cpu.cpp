#include "cpu.h"

#define TEST 0xdeadbeef

void CPU::test_ram(void) {
	unsigned int data = TEST;
	tlm::tlm_generic_payload* tr = new tlm::tlm_generic_payload();
	tr->set_command(tlm::TLM_WRITE_COMMAND);
	tr->set_address(0x00);
	tr->set_data_ptr((unsigned char*) &data);
	tr->set_data_length(sizeof(TEST));
	tr->set_streaming_width(sizeof(TEST));
	tr->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = SC_ZERO_TIME;

	CPU::socket->b_transport(*tr, delay);

	if(tr->is_response_error()) {
		cerr << "Error: " + tr->get_response_string() << endl;
		return;
	}

	tr->set_command(tlm::TLM_READ_COMMAND);

	unsigned int read_bytes = CPU::socket->transport_dbg(*tr);

	if(read_bytes != sizeof(TEST)) {
		cerr << "Error: transport_dbg failed, read " << read_bytes << " bytes" << endl;
		return;
	}
	if(data != TEST) {
		cerr << "Error: response doesn't match" << endl;
		return;
	}

	cout << "Test RAM OK." << endl;
};
