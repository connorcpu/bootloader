#include <stdint.h>

void drawRect(uint8_t _rgb[], uint8_t* where, uint16_t width, uint16_t height, uint8_t* buffer);
void drawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, uint8_t _rgb[], uint8_t* buffer);
void putPixel(int64_t x, int64_t y, uint8_t _rgb[], uint8_t* buffer);
