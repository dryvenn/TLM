#include <fstream>
#include <string>
#include "master.h"


void Master::invalidate_direct_mem_ptr(sc_dt::uint64 start, sc_dt::uint64 end)
{
	(void) start; (void) end;

	if(Master::dmi != NULL)
		delete this->dmi;
}


void Master::test_ram(void) {
	const unsigned int test = 0xdeadbeef;
	unsigned int data = test;
	const unsigned int byte_en = 0xffffff00;
	this->payload->set_command(tlm::TLM_WRITE_COMMAND);
	this->payload->set_address(0x00);
	this->payload->set_data_ptr((unsigned char*) &data);
	this->payload->set_data_length(sizeof(test));
	this->payload->set_byte_enable_ptr((unsigned char*) &byte_en);
	this->payload->set_byte_enable_length(sizeof(byte_en));
	this->payload->set_streaming_width(sizeof(test));
	this->payload->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = SC_ZERO_TIME;

	this->socket->b_transport(*this->payload, delay);

	if(this->payload->is_response_error()) {
		cerr << "Error: " + this->payload->get_response_string() << endl;
		return;
	}

	this->payload->set_command(tlm::TLM_READ_COMMAND);

	unsigned int read_bytes = this->socket->transport_dbg(*this->payload);

	if(read_bytes != sizeof(test)) {
		cerr << "Error: transport_dbg failed, read " << read_bytes << " bytes" << endl;
		return;
	}
	if(data != (test & byte_en)) {
		cerr << "Error: response doesn't match" << endl;
		return;
	}

	cout << "Test RAM OK." << endl;
}


int Master::read(unsigned int addr, unsigned char* buff, unsigned int size)
{
	this->payload->set_command(tlm::TLM_READ_COMMAND);
	this->payload->set_address(addr);
	this->payload->set_data_ptr(buff);
	this->payload->set_data_length(size);
	this->payload->set_byte_enable_ptr(NULL);
	this->payload->set_streaming_width(size);
	this->payload->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = SC_ZERO_TIME;

	this->socket->b_transport(*this->payload, delay);

	this->sleep(delay);

	return this->payload->is_response_error() ? -1 : 0;
}


int Master::write(unsigned int addr, unsigned char* buff, unsigned int size)
{
	this->payload->set_command(tlm::TLM_WRITE_COMMAND);
	this->payload->set_address(addr);
	this->payload->set_data_ptr(buff);
	this->payload->set_data_length(size);
	this->payload->set_byte_enable_ptr(NULL);
	this->payload->set_streaming_width(size);
	this->payload->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = SC_ZERO_TIME;

	this->socket->b_transport(*this->payload, delay);

	this->sleep(delay);

	return this->payload->is_response_error() ? -1 : 0;
}


int Master::read_byte(unsigned int addr, unsigned char& byte)
{
	return this->read(addr, &byte, 1);
}


int Master::write_byte(unsigned int addr, unsigned char& byte)
{
	return this->write(addr, &byte, 1);
}


void Master::sleep(sc_time duration)
{
	this->q_keeper.inc(duration);
	if(this->q_keeper.need_sync())
		this->q_keeper.sync();
}
