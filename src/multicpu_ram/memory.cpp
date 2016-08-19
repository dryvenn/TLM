#include "memory.h"

void Memory::b_transport(tlm::tlm_generic_payload& tr, sc_time& delay) {
	tlm::tlm_command cmd = tr.get_command();
	unsigned int addr = tr.get_address();
	unsigned char* data = tr.get_data_ptr();
	unsigned int size = tr.get_data_length();
	unsigned char* be_ptr = tr.get_byte_enable_ptr();
	unsigned int be_size = tr.get_byte_enable_length();
	unsigned int stream_width = tr.get_streaming_width();

	// Streaming is not supported.
	if(!size || stream_width != size) {
		tr.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		return;
	}
	// DMI is supported
	tr.set_dmi_allowed(true);
	// Check the address and size.
	if(addr + size > this->get_length()) {
		tr.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		return;
	}

	if(cmd == tlm::TLM_READ_COMMAND) {
		delay += this->read_latency;

		if(be_ptr == NULL)
			this->read(addr, data, size);
		else
			for(unsigned int i = 0; i < size; i++)
				if(be_ptr[i % be_size] == TLM_BYTE_ENABLED)
					this->read_byte(addr + i, data[i]);
	}
	else if(cmd == tlm::TLM_WRITE_COMMAND) {
		delay += this->write_latency;

		if(be_ptr == NULL)
			this->write(addr, data, size);
		else
			for(unsigned int i = 0; i < size; i++)
				if(be_ptr[i % be_size] == TLM_BYTE_ENABLED)
					this->write_byte(addr + i, data[i]);
	}
	// else cmd == tlm::TLM_IGNORE_COMMAND;

	tr.set_response_status(tlm::TLM_OK_RESPONSE);
}

unsigned int Memory::transport_dbg(tlm::tlm_generic_payload& tr) {
	tlm::tlm_command cmd = tr.get_command();
	unsigned int addr = tr.get_address();
	unsigned char* data = tr.get_data_ptr();
	unsigned int size = tr.get_data_length();
	size = addr + size > this->get_length() ? this->get_length() - addr : size;

	if(cmd == tlm::TLM_READ_COMMAND)
		this->read(addr, data, size);
	else if(cmd == tlm::TLM_WRITE_COMMAND)
		this->write(addr, data, size);
	// else cmd == tlm::TLM_IGNORE_COMMAND;

	return size;
}

bool Memory::get_direct_mem_ptr(tlm::tlm_generic_payload& tr, tlm::tlm_dmi& dmi)
{
	(void) tr;

	dmi.allow_read_write();
	dmi.set_dmi_ptr(this->get_memory_ptr());
	dmi.set_start_address(0);
	dmi.set_end_address(this->get_length() - 1);
	dmi.set_read_latency(SC_ZERO_TIME);
	dmi.set_write_latency(SC_ZERO_TIME);

	return true;
}

void Memory::dump_on_stdout(void)
{
	unsigned int j = 0;
	unsigned char* data = this->get_memory_ptr();
	cout << hex;
	for(;;) {
		for(unsigned int i = 0; i < 10; i++) {
			if(j >= this->get_length()) {
				cout << endl;
				return;
			}
			else {
				cout << "0x" << (unsigned int) data[j] << " ";
				++j;
			}
		}
		cout << endl;
	}
}
