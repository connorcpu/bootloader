#include <stdint.h>

/*typedef struct gdtEntry{

   uint16_t limit;
   uint16_t base;
   uint8_t Morebase;
   uint8_t access;
   uint16_t limitFlagsBase;


}__attribute__((packed)) gdtEntry_t;*/

typedef struct gdtEntry{

   unsigned int limitLow      : 16;
   unsigned int baseLow       : 24;
   unsigned int accessed      : 1;
   unsigned int read_write    : 1;
   unsigned int conforming    : 1; //no clue
   unsigned int code          : 1; //1 for code, otherwise data
   unsigned int codeOrData    : 1; //1 unless TSS or LDT
   unsigned int ring          : 2; //privilege level
   unsigned int present       : 1; //exists or not
   unsigned int limitHigh     : 4;
   unsigned int available     : 1; //software only
   unsigned int longT         : 1; //1 means yes
   unsigned int big           : 1; //32 bit opcodes?? whaaaat?
   unsigned int granularity   : 1; //1 for 4k page addressing, 0 for byte
   unsigned int baseHigh      : 8;


}__attribute__((packed)) gdtEntry_t;

typedef struct gdtSysExtension{

   uint32_t baseMegaHigh: 32;
   uint32_t reserved: 32;

}__attribute__((packed)) gdtSysExtension_t;

typedef struct gdtrPointer{

   uint16_t size;
   uint64_t gdtAddr;

}__attribute__((packed)) gdtrPointer_t;

void loadGDT();
void runLGDTR();
gdtEntry_t encodeGdtEntry(uint8_t type, uint8_t ring);
