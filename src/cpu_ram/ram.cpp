#include "ram.h"

void RAM::b_transport(tlm::tlm_generic_payload& tr, sc_time& delay) {
	(void) delay; // Timing does't matter for now;

	tlm::tlm_command cmd = tr.get_command();
	unsigned int addr = tr.get_address();
	unsigned char* data = tr.get_data_ptr();
	unsigned int size = tr.get_data_length();
	unsigned char* byte_en = tr.get_byte_enable_ptr();
	unsigned int stream_width = tr.get_streaming_width();

	// Byte enables are not supported.
	if(byte_en) {
		tr.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		return;
	}
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

	switch(cmd) {
		case tlm::TLM_READ_COMMAND:
			memcpy(data, &RAM::data[addr], size);
			break;
		case tlm::TLM_WRITE_COMMAND:
			memcpy(&RAM::data[addr], data, size);
			break;
		default:
			tr.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
			return;
	}
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
