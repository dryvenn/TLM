#ifndef __RAM_H__
#define __RAM_H__

#include <systemc.h>
#include "memory.h"
#include "map.h"


class RAM: public Memory {
	private:
		unsigned char data[RAM_SIZE];

	public:
		RAM(sc_module_name name, sc_time latency): Memory(name, latency, latency) {
			memset(data, 0, this->get_length());
		}

		virtual unsigned int get_length(void) { return RAM_SIZE; }
		virtual unsigned char* get_memory_ptr(void) { return (unsigned char*) this->data; }
		virtual inline bool is_dmi_supported(void) { return true; }
		virtual void read(const unsigned int addr, unsigned char* const buff, const unsigned int size);
		virtual void write(const unsigned int addr, unsigned char* const buff, const unsigned int size);
};

#endif
