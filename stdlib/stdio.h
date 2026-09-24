#include <stdarg.h>
#include <stdint.h>
#include "stdbool.h"

int ioInit();
//int putch(char character, uint8_t x, uint8_t y);
int putch(char character);
int putst(char* string, uint8_t x, uint8_t y);
int print(char* string);
void printch(char ch);
void kprintf(char *string, ...);
void format(char *string, va_list args);
void i2h(uint64_t decn, char* buffer);
void i2a(uint64_t num, char* buffer);
char* readLine(uint8_t fd);
float a2f(char* str);
void f2a(float x, char* p);
char k2a(uint8_t keycode);
