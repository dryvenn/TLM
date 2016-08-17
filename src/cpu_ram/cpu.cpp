#include "cpu.h"

void CPU::process(void)
{
	this->execute_instruction_file();
}

void CPU::execute_instruction_file(void) {
	std::ifstream infile(INSTRUCTION_FILE);
	std::string line;
	while (std::getline(infile, line)) {
		cout << line << endl;

		q_keeper.inc(sc_time(5, SC_NS));
		if(q_keeper.need_sync())
			q_keeper.sync();
	}
}
