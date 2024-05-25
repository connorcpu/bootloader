#include <stdint.h>
#include "utils.h"

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

typedef struct {

   uint64_t ds;
   uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
   uint64_t int_no, err_code;
   uint64_t rip, cs, eflags, userrsp, ss;

} registers_t;
typedef void (*isr_t)(registers_t);

void createTable(void);
void exception_handler(registers_t r);
void irq_handler(registers_t r);
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void registerInterupt(uint8_t vector, isr_t handler);


