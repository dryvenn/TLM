#ifndef __CPU_H__
#define __CPU_H__

#include "master.h"

#define REG_NUM 8

typedef unsigned int reg_num;
typedef unsigned char word;


class CPU: public Master {
	private:
		word R[REG_NUM];
		std::string instr_file;
		sc_time instr_latency;
	public:
		CPU(sc_module_name name, std::string instr_file, sc_time instr_latency): Master(name), instr_file(instr_file), instr_latency(instr_latency) {
		}

		virtual void process(void);
		virtual void execute_instr_file(void);
		virtual void interpret_command(std::string command);
		/* Processor operations */
		void dmp(void);
		void nop(void);
		void chr(reg_num dst, word data);
		void ldr(reg_num dst, unsigned int addr);
		void str(reg_num src, unsigned int addr);
		void add(reg_num dst, reg_num opd1, reg_num opd2);
		void sub(reg_num dst, reg_num opd1, reg_num opd2);
		void mul(reg_num dst, reg_num opd1, reg_num opd2);
		void div(reg_num dst, reg_num opd1, reg_num opd2);
};

#endif
