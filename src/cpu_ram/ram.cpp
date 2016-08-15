#include "ram.h"

void RAM::b_transport(tlm::tlm_generic_payload& tr, sc_time& delay) {
	(void) delay; // Timing does't matter for now;

	tlm::tlm_command cmd = tr.get_command();
	unsigned int addr = tr.get_address();
	unsigned char* data = tr.get_data_ptr();
	unsigned int len = tr.get_data_length();
	unsigned char* byte_en = tr.get_byte_enable_ptr();
	unsigned int stream_width = tr.get_streaming_width();

	// Byte enables are not supported.
	if(byte_en) {
		tr.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		return;
	}
	// Streaming is not supported.
	if(!len || stream_width != len) {
		tr.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		return;
	}
	// Check the address and length.
	if(addr + len > RAM::len) {
		tr.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		return;
	}

	switch(cmd) {
		case tlm::TLM_READ_COMMAND:
			memcpy(data, &RAM::data[addr], len);
			break;
		case tlm::TLM_WRITE_COMMAND:
			memcpy(&RAM::data[addr], data, len);
			break;
		default:
			tr.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
			return;
	}
	tr.set_response_status(tlm::TLM_OK_RESPONSE);
}
