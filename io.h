#include <stdint.h>
#include <stdbool.h>

#define VIDEO_ADDRESS 0xb8000

int ioInit();
int putch(char character, uint8_t x, uint8_t y);
int putst(char* string, uint8_t y);
int print(char* string);
void printch(char ch);
void backspace();
void enter();
void cls();
void setShift(bool enable);
void movCursor(uint8_t x, uint8_t y);

