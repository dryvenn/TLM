#ifndef __MAP_H__
#define __MAP_H__

#define MAP_SEPARATOR 0x10000000

#define RAM_ID 0
#define RAM_SIZE 256

#define GPIO_ID 1
#define GPIO_SIZE 64


static inline void encode_address(unsigned int& addr, int id)
{
	addr += id * MAP_SEPARATOR;
}

static inline int decode_address(unsigned int& addr)
{
	unsigned int id = addr / MAP_SEPARATOR;
	addr %= MAP_SEPARATOR;
	return id;
}

#endif
