#include <stdint.h>

#define PML4ADDR 0x1000

typedef struct{

   uint64_t base;
   uint64_t length;
   uint32_t type;
   uint32_t ACPI;
   
} E820MemBlock ;

enum E820MemBlockType {

   E820_USABLE = 1,
   E820_RESERVED = 2,
   E820_ACPI_RECLAIMABLE = 2,
   E820_ACPI_NVS = 4,
   E820_BAD_MEMORY = 5,


};

void pagingInit();
uint8_t* kmalloc(uint32_t size);
uint8_t mappage(uint64_t* pml4Ptr, uint8_t* physAddr, uint8_t* virtAddr, uint16_t flags);
uint8_t mapPage(uint8_t* physAddr, uint8_t* virtAddr, uint16_t flags);
void* alloc_page(void);
