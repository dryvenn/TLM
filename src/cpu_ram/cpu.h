#ifndef __CPU_H__
#define __CPU_H__

#include "master.h"

typedef unsigned int reg;
typedef unsigned char word;


class CPU: public Master {
	private:
		word R[8];
	public:
		CPU(): Master("CPU") {
			(void) R;
		}

		virtual void process(void);
		virtual void execute_instruction_file(void);
};

#endif
