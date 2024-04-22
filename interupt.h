#include <stdint.h>

typedef struct {
   
   uint16_t isr_low;    //lower 16bits of handlers address
   uint16_t kernel_cs;  //the segment selector to be loaded into cs before calling the handler
   uint8_t  ist;        //huh
   uint8_t attributes;  //type and attribute of the interupt, see ovdev idt page
   uint16_t isr_mid;    //higher 16 bits of handlers address
   uint32_t isr_high;   //higher 32 bits of handlers address
   uint32_t reserved;   //set to 0

} __attribute__((packed)) idt_entry_t;

typedef struct {

   uint16_t limit;
   uint64_t base;

} __attribute__((packed)) idtr_t;
void createTable(void);
void registerInterupt(uint8_t vector, void* handler);
__attribute__((noreturn))
void exception_handler(void);
__attribute__((noreturn))
void keyboardHandler();
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
