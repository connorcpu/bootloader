#include <stdint.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void insl(int32_t port, void *addr, int32_t cnt);
void io_wait(void);

