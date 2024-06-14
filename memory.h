#include <stdint.h>

#define PML4ADDR 0x1000

void pagingInit();
uint32_t kmalloc(uint32_t size, uint32_t* physAddr);
