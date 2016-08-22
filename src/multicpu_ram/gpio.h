#ifndef __GPIO_H__
#define __GPIO_H__

#include <systemc.h>
#include "memory.h"
#include "map.h"

class GPIO: public Memory {
	private:
		unsigned char data[GPIO_SIZE / 8];

	public:
		GPIO(sc_module_name name, sc_time read_latency, sc_time write_latency): Memory(name, read_latency, write_latency) {
			memset(data, 0, this->get_length());
		}

		virtual unsigned int get_length(void) { return GPIO_SIZE; }
		virtual inline unsigned char* get_memory_ptr(void) { return NULL; }
		virtual inline bool is_dmi_supported(void) { return false; }
		virtual void read(const unsigned int addr, unsigned char* const buff, const unsigned int size);
		virtual void write(const unsigned int addr, unsigned char* const buff, const unsigned int size);
};

#endif
