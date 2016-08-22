#include "cpu.h"


void CPU::process(void)
{
	this->execute_instr_file();
}


void CPU::execute_instr_file(void) {
	std::ifstream infile(this->instr_file);
	std::string line;
	while (std::getline(infile, line))
		interpret_command(line);
}


unsigned int get_uint(std::string opd)
{
	assert(opd.length() > 0);
	return (unsigned int) std::stoi(opd, NULL, 0);
}


word get_word(std::string opd)
{
	return (word) get_uint(opd);
}


reg_num get_reg_num(std::string opd)
{
	assert(opd.at(0) == 'r');
	return get_uint(opd.substr(1));
}


void CPU::interpret_command(std::string command)
{
	// Don't interpret empty lines
	if(command.length() == 0)
		return;
	// Basic support for comments
	else if(command.at(0) == '#')
		return;
	else
		cout << this->name() << ": " << command << endl;

	std::string op;
	std::string opds[10];
	unsigned int operand_num = 0;
	unsigned int index = 0;
	// operator
	index = command.find(' ');
	if(index > command.length())
		index = command.length();
	op = command.substr(0, index);
	// operands
	while(index < command.length() - 1) {
		index++; // pass the whitespace
		unsigned int start = index;
		index = command.find(' ', start);
		if(index > command.length())
			index = command.length();
		opds[operand_num] = command.substr(start, index - start);
		operand_num++;
	}

	if(!std::strcmp("nop", op.c_str())) {
		assert(operand_num == 0);
		this->nop();
	}
	else if(!std::strcmp("dmp", op.c_str())) {
		assert(operand_num == 0);
		this->dmp();
	}
	else if(!std::strcmp("chr", op.c_str())) {
		assert(operand_num == 2);
		reg_num dst = get_reg_num(opds[0]);
		word data = get_word(opds[1]);
		this->chr(dst, data);
	}
	else if(!std::strcmp("ldr", op.c_str())) {
		assert(operand_num == 2);
		reg_num dst = get_reg_num(opds[0]);
		unsigned int addr = get_uint(opds[1]);
		this->ldr(dst, addr);
	}
	else if(!std::strcmp("str", op.c_str())) {
		assert(operand_num == 2);
		reg_num src = get_reg_num(opds[0]);
		unsigned int addr = get_uint(opds[1]);
		this->str(src, addr);
	}
	else if(!std::strcmp("add", op.c_str())) {
		assert(operand_num == 3);
		reg_num dst = get_reg_num(opds[0]);
		reg_num opd1 = get_reg_num(opds[1]);
		reg_num opd2 = get_reg_num(opds[2]);
		this->add(dst, opd1, opd2);
	}
	else if(!std::strcmp("sub", op.c_str())) {
		assert(operand_num == 3);
		reg_num dst = get_reg_num(opds[0]);
		reg_num opd1 = get_reg_num(opds[1]);
		reg_num opd2 = get_reg_num(opds[2]);
		this->sub(dst, opd1, opd2);
	}
	else if(!std::strcmp("mul", op.c_str())) {
		assert(operand_num == 3);
		reg_num dst = get_reg_num(opds[0]);
		reg_num opd1 = get_reg_num(opds[1]);
		reg_num opd2 = get_reg_num(opds[2]);
		this->mul(dst, opd1, opd2);
	}
	else if(!std::strcmp("div", op.c_str())) {
		assert(operand_num == 3);
		reg_num dst = get_reg_num(opds[0]);
		reg_num opd1 = get_reg_num(opds[1]);
		reg_num opd2 = get_reg_num(opds[2]);
		this->div(dst, opd1, opd2);
	}
	else {
		throw "Unknown operator";
	}

	this->sleep(this->instr_latency);
}


void CPU::nop()
{
	// nothing to do here
}


void CPU::chr(reg_num dst, word data)
{
	R[dst] = data;
}


void CPU::dmp()
{
	for(unsigned int i = 0; i < REG_NUM; i++) {
		cout << " r" << dec << i << ": ";
		cout << "0x" << hex << (unsigned int) R[i];
	}
	cout << endl;
}


void CPU::ldr(reg_num dst, unsigned int addr)
{
	assert(!this->read_byte(addr, R[dst]));
}


void CPU::str(reg_num src, unsigned int addr)
{
	assert(!this->write_byte(addr, R[src]));
}


void CPU::add(reg_num dst, reg_num opd1, reg_num opd2)
{
	R[dst] = R[opd1] + R[opd2];
}


void CPU::sub(reg_num dst, reg_num opd1, reg_num opd2)
{
	R[dst] = R[opd1] - R[opd2];
}


void CPU::mul(reg_num dst, reg_num opd1, reg_num opd2)
{
	R[dst] = R[opd1] * R[opd2];
}


void CPU::div(reg_num dst, reg_num opd1, reg_num opd2)
{
	R[dst] = R[opd1] / R[opd2];
}
