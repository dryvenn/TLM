#ifndef __MAP_H__
#define __MAP_H__

/* These addresses reflect the mapping on the interconnect.
 * Sizes are in byte.
 */

#define RAM_ADDR 0x10000000
#define RAM_SIZE 256

#define GPIO_ADDR 0x20000000
// One byte per GPIO.
#define GPIO_SIZE 64

#endif
