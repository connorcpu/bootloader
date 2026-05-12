#include <stdint.h>
void startfd();
uint8_t registerfd(void* fileLoc);
void* retrievefd(uint8_t fd);
