#include <stdint.h>

typedef struct tss{

   uint32_t res1;
   uint32_t RSP0_l;
   uint32_t RSP0_h;
   uint32_t RSP1_l;
   uint32_t RSP1_h;
   uint32_t RSP2_l;
   uint32_t RSP2_h;
   uint64_t res2;
   uint32_t IST1_l;
   uint32_t IST1_h;
   uint32_t IST2_l;
   uint32_t IST2_h;
   uint32_t IST3_l;
   uint32_t IST3_h;
   uint32_t IST4_l;
   uint32_t IST4_h;
   uint32_t IST5_l;
   uint32_t IST5_h;
   uint32_t IST6_l;
   uint32_t IST6_h;
   uint32_t IST7_l;
   uint32_t IST7_h;
   uint64_t res3;
   uint16_t res4;
   uint16_t IOPB;

   //RSP0, 1 & 2 are the stack pointers per ring
   //ISTn are the stack pointer for IDT execution, the number is listed in the IDT entry
   //IOPB: I/O map base address field, offset from the tss base for the IO permission bit map, whatever that may be

}__attribute__((packed)) tss_t;
void setTss();
