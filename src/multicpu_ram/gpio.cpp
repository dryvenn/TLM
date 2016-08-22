#include "gpio.h"


void GPIO::read(const unsigned int addr, unsigned char* const buff, const unsigned int size)
{
	for(unsigned int i = 0; i < size; i++) {
		unsigned int caddr = addr + i;
		buff[i] = 1 && ( data[caddr / 8] & (1 << (caddr % 8)) );
	}
}


void GPIO::write(const unsigned int addr, unsigned char* const buff, const unsigned int size)
{
	for(unsigned int i = 0; i < size; i++) {
		unsigned int caddr = addr + i;
		data[caddr / 8] =
			( data[caddr / 8] & (~(1<<(caddr % 8))) ) +
			( (1 && buff[i]) << (caddr % 8) );
	}
}
