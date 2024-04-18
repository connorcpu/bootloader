#include <stdint.h>

#define VIDEO_ADDRESS 0xb8000

int ioInit();
int putch(char character, uint8_t x, uint8_t y);
int putst(char* string, uint8_t y);
int print(char* string);

