#include "cpu.h"

// TODO: dmi, operations
void CPU::test_ram(void) {
		cout << "Hello, test!" << endl;
	unsigned int len = 4;
	unsigned char* data = (unsigned char *) malloc(len);
	tlm::tlm_generic_payload* tr = new tlm::tlm_generic_payload();
	tr->set_command(tlm::TLM_READ_COMMAND);
	tr->set_address(0x00);
	tr->set_data_ptr(data);
	tr->set_data_length(len);
	tr->set_streaming_width(len);
	tr->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = sc_time(1, SC_PS);

	CPU::socket->b_transport(*tr, delay);

	if(tr->is_response_error()) {
		cerr << "Error: " + tr->get_response_string() << endl;
		return;
	}

	for(unsigned int i = 0; i < len; i++) {
		if(data[i] != 0xff) {
			cerr << "Error: response doesn't match" << endl;
			return;
		}
	}

	cout << "Test RAM OK." << endl;
};
