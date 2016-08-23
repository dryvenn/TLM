#include "ram.h"


void RAM::read(const unsigned int addr, unsigned char* const buff, const unsigned int size)
{
	memcpy(buff, data + addr, size);
}


void RAM::write(const unsigned int addr, unsigned char* const buff, const unsigned int size)
{
	memcpy(data + addr, buff, size);
}
