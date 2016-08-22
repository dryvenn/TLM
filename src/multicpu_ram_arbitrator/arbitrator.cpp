#include "arbitrator.h"


void Arbitrator::reset_enlisted()
{
	memset(this->enlisted, 0, sizeof(bool) * MAX_MASTER_NB);
}

bool Arbitrator::has_quantum_nb_changed()
{
	static const sc_time global_quantum = tlm::tlm_global_quantum::instance().get();
	quantum_nb actual =  (quantum_nb) (sc_time_stamp() / global_quantum);

	if(this->current == actual)
		return false;
	this->current = actual;
	return true;
}


void Arbitrator::elect()
{
	this->last_token_master = this->token_master;
	this->token_master = NO_MASTER;
	// Do it in a round-robin fashion
	for(unsigned int i = 0; i < this->master_num; i++)
		if(this->enlisted[i])
			if(this->last_token_master >= this->master_num - 1 ||
					this->last_token_master < i) {
				this->token_master = i;
				break;
			}
	this->reset_enlisted();
}


void Arbitrator::enlist(unsigned int id)
{
	assert(id < MAX_MASTER_NB);
	this->enlisted[id] = true;
}


/* 1- Elect the new token owner based on the previous quantum (no owner during the first one).
 * 2- For those who don't have access and want it, enlist them for the next quantum.
 * 3- For the one who has access, do the transporting but do not enlist him.
 */
void Arbitrator::process_transaction(int id, tlm::tlm_generic_payload& tr)
{
	unsigned int uid = (unsigned int) id;

	// New quantum implies elections
	if(this->has_quantum_nb_changed())
		this->elect();

	if(uid != this->token_master) {
		this->enlist(uid);
		tr.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
	}
}
