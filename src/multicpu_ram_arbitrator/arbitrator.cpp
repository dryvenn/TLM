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


void Arbitrator::enlist(int id)
{
	assert(id >= 0);
	assert(id < MAX_MASTER_NB);
	this->enlisted[id] = true;
}

/* If we're in a new quantum, choose which master's gonna have the token this time.
 * Let masters enlist for having the token next time.
 * Make the accesses of the current token owner.
 */
void Arbitrator::process_transaction(int id, tlm::tlm_generic_payload& tr)
{
	// New quantum implies elections
	if(this->has_quantum_nb_changed())
		this->elect();

	// Upper bit at 1 means enlisting for next quantum
	if(tr.get_address() & ARB_REGISTER_MASK)
		this->enlist(id);
	// Put the right address back
	tr.set_address(tr.get_address() & (~ARB_REGISTER_MASK));

	// All ones (except this upper bit) means enlisting only
	if(tr.get_address() == ARB_REGISTER_ONLY_VAL)
		tr.set_response_status(tlm::TLM_OK_RESPONSE);
	// Otherwise if access is denied say it with the command error
	else if((unsigned int) id != this->token_master)
		tr.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
	// else access is authorized and will happen
}
