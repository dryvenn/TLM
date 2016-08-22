#ifndef __ARBITRATOR_H__
#define __ARBITRATOR_H__

#include <systemc.h>
#include <tlm.h>

#define NO_MASTER ((unsigned int) -1 )
#define MAX_MASTER_NB 32


typedef unsigned long quantum_nb;


class Arbitrator {
	private:
		const unsigned int master_num;
		unsigned int token_master;
		bool enlisted[MAX_MASTER_NB];
		unsigned int last_token_master;
		quantum_nb current;

	public:
		Arbitrator(unsigned int master_num): master_num(master_num) {
			this->reset_enlisted();
			token_master = NO_MASTER;
			last_token_master = NO_MASTER;
			current = 0;
		}

		virtual void reset_enlisted(void);
		virtual bool has_quantum_nb_changed(void);
		virtual void elect(void);
		virtual void enlist(unsigned int);
		virtual void process_transaction(int, tlm::tlm_generic_payload&);
};

#endif
