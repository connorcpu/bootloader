#include <stdarg.h>
#include <stdint.h>
#include "stdbool.h"
#include "utils.h"

#define VIDEO_ADDRESS 0xb8000

int ioInit();
int putch(char character, uint8_t x, uint8_t y);
int putst(char* string, uint8_t x, uint8_t y);
int print(char* string);
void printch(char ch);
void backspace();
void enter();
void cls();
void setmod(uint8_t modkeyindx, bool enable);
void movCursor(uint8_t x, uint8_t y);
void kprintf(char *string, ...);
void format(char *string, va_list args);
void i2a(uint64_t num, char* buffer);
