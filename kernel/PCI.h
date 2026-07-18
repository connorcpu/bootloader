#include <stdint.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

typedef struct pciConfig{

   uint8_t enable    : 1;
   uint8_t res       : 7;
   uint8_t busNum    : 8;
   uint8_t devNum    : 5;
   uint8_t funcNum   : 3;
   uint8_t regOffset : 8;

}__attribute__((packed)) pciConfig_t;

void pciDetectAll();
