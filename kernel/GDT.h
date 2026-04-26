#include <stdint.h>

typedef struct gdtEntry{

   uint16_t limit;
   uint16_t base;
   uint8_t Morebase;
   uint8_t access;
   uint16_t limitFlagsBase;


}__attribute__((packed)) gdtEntry_t;

typedef struct gdtCEntry{

   uint32_t limit;
   uint32_t base;
   uint8_t accessByte;
   uint8_t flags;

} gdtCEntry_t;

typedef struct gdtrPointer{

   uint64_t gdtAddr;
   uint16_t size;

}__attribute__((packed)) gdtrPointer_t;

void loadGDT();
void runLGDTR(gdtrPointer_t ptr);
gdtEntry_t encodeGdtEntry(gdtCEntry_t source);
