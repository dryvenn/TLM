#include "ram.h"


unsigned int RAM::get_length()
{
	return length;
}


unsigned char* RAM::get_memory_ptr()
{
	return (unsigned char*) data;
}


int RAM::read(unsigned int addr, unsigned char* buff, unsigned int size)
{
	if(addr + size > length)
		return -1;

	memcpy(buff, data, size);

	return 0;
}


int RAM::write(unsigned int addr, unsigned char* buff, unsigned int size)
{
	if(addr + size > length)
		return -1;

	memcpy(data, buff, size);

	return 0;
}


int RAM::read_byte(unsigned int addr, unsigned char& byte)
{
	if(addr + 1 > length)
		return -1;

	byte = data[addr];

	return 0;
}


int RAM::write_byte(unsigned int addr, unsigned char& byte)
{
	if(addr + 1 > length)
		return -1;

	data[addr] = byte;

	return 0;
}
