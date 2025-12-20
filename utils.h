#include <stdint.h>

void invlpg(void* addr);
void load_cr3(void * cr3_value);
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void insl(int32_t port, void *addr, int32_t cnt);
void io_wait(void);
int tolower(int character); 
int toupper(int character); 
int strcmp (const char* str1, const char* str2);
