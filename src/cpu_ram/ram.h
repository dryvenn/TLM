#ifndef __RAM_H__
#define __RAM_H__

#include <systemc.h>
#include "memory.h"

#define LENGTH 256


class RAM: public Memory {
	private:
		unsigned int length;
		unsigned char data[LENGTH];

	public:
		RAM(): Memory("RAM"), length(LENGTH)
		{
			memset(data, 0, length);
		}

		virtual unsigned int get_length(void);
		virtual unsigned char* get_memory_ptr(void);
		virtual int read(unsigned int addr, unsigned char* buff, unsigned int size);
		virtual int write(unsigned int addr, unsigned char* buff, unsigned int size);
		virtual int read_byte(unsigned int addr, unsigned char& byte);
		virtual int write_byte(unsigned int addr, unsigned char& byte);
};


#endif
