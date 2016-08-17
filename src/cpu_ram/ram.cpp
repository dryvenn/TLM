#include "ram.h"

void RAM::b_transport(tlm::tlm_generic_payload& tr, sc_time& delay) {
	(void) delay; // Timing does't matter for now;

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
	if(addr + size > RAM::size) {
		tr.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		return;
	}

	if(cmd == tlm::TLM_READ_COMMAND) {
		for(unsigned int i = 0; i < size; i++)
			if(be_ptr == NULL || be_ptr[i % be_size] == TLM_BYTE_ENABLED)
				data[i] = RAM::data[addr + i];
	}
	else if(cmd == tlm::TLM_WRITE_COMMAND) {
		for(unsigned int i = 0; i < size; i++)
			if(be_ptr == NULL || be_ptr[i % be_size] == TLM_BYTE_ENABLED)
				RAM::data[addr + i] = data[i];
	}
	// else cmd == tlm::TLM_IGNORE_COMMAND;

	tr.set_response_status(tlm::TLM_OK_RESPONSE);
}

unsigned int RAM::transport_dbg(tlm::tlm_generic_payload& tr) {
	tlm::tlm_command cmd = tr.get_command();
	unsigned int addr = tr.get_address();
	unsigned char* data = tr.get_data_ptr();
	unsigned int size = tr.get_data_length();
	size = addr + size > RAM::size ? RAM::size - addr : size;
	switch(cmd) {
		case tlm::TLM_READ_COMMAND:
			memcpy(data, &RAM::data[addr], size);
			break;
		case tlm::TLM_WRITE_COMMAND:
			memcpy(&RAM::data[addr], data, size);
			break;
		default:
			return 0;
	}
	return size;
}

bool RAM::get_direct_mem_ptr(tlm::tlm_generic_payload& tr, tlm::tlm_dmi& dmi)
{
	(void) tr;

	dmi.allow_read_write();
	dmi.set_dmi_ptr(RAM::data);
	dmi.set_start_address(0);
	dmi.set_end_address(RAM::size-1);
	dmi.set_read_latency(SC_ZERO_TIME);
	dmi.set_write_latency(SC_ZERO_TIME);

	return true;
}
