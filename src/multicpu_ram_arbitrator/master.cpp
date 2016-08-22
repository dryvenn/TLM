#include <fstream>
#include <string>
#include "master.h"
#include "arbitrator.h"


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

	this->socket->b_transport(*tr, delay);

	if(tr->is_response_error()) {
		cerr << "Error: " + tr->get_response_string() << endl;
		return;
	}

	tr->set_command(tlm::TLM_READ_COMMAND);

	unsigned int read_bytes = this->socket->transport_dbg(*tr);

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




void Master::transport(tlm::tlm_generic_payload& tr, sc_time& delay)
{
	for(;;) {
		// reset the response status to init value
		tr.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
		// request access
		tr.set_address(tr.get_address() | ARB_REGISTER_MASK);
		// do transporting
		this->socket->b_transport(tr, delay);

		if(tr.get_response_status() != tlm::TLM_COMMAND_ERROR_RESPONSE)
			break;

		// wait for next turn
		this->fast_forward();
	}
}


int Master::read(unsigned int addr, unsigned char* buff, unsigned int size)
{
	tlm::tlm_generic_payload* tr = new tlm::tlm_generic_payload();
	tr->set_command(tlm::TLM_READ_COMMAND);
	tr->set_address(addr);
	tr->set_data_ptr(buff);
	tr->set_data_length(size);
	tr->set_byte_enable_ptr(NULL);
	tr->set_streaming_width(size);
	tr->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = SC_ZERO_TIME;

	this->transport(*tr, delay);

	this->sleep(delay);

	return tr->is_response_error() ? -1 : 0;
}


int Master::write(unsigned int addr, unsigned char* buff, unsigned int size)
{
	tlm::tlm_generic_payload* tr = new tlm::tlm_generic_payload();
	tr->set_command(tlm::TLM_WRITE_COMMAND);
	tr->set_address(addr);
	tr->set_data_ptr(buff);
	tr->set_data_length(size);
	tr->set_byte_enable_ptr(NULL);
	tr->set_streaming_width(size);
	tr->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_time delay = SC_ZERO_TIME;

	this->transport(*tr, delay);

	this->sleep(delay);

	return tr->is_response_error() ? -1 : 0;
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


void Master::fast_forward()
{
	sc_time local_q = tlm::tlm_global_quantum::instance().compute_local_quantum();
	this->sleep(local_q);
}
