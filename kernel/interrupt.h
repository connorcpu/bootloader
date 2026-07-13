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

typedef struct {

//unsigned int sgx : 1, : 8, ss : 1, pk : 1, infet : 1, resw : 1, u : 1, w : 1, p : 1;

   //char bs : 1, ss : 1, pk : 1, infet : 1, resw : 1, u : 1, w : 1, p : 1;
   char p : 1, w : 1, u : 1, resw : 1, infet : 1, pk : 1, ss : 1, bs : 1;
   char sgx : 8;
   /*int sgx : 1;
   int res : 8;
   int ss : 1;
   int pk : 1;
   int infet : 1;
   int resw : 1;
   int u : 1;
   int w : 1;
   int p : 1;*/


}__attribute__((packed)) pageFaultError_t;
//;} pageFaultError_t;

typedef struct {

   uint64_t ds;
   //no clue why but the combined line is incorrect
   uint64_t r11, r10, r9, r8;
   uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
   //uint64_t r11, r10, r9, r8, rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
   uint64_t int_no, err_code;
   uint64_t rip, cs, eflags, userrsp, ss;

} registers_t;
typedef void (*isr_t)(registers_t);

void createIDT(void);
void exception_handler(registers_t r);
void irq_handler(registers_t r);
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void registerInterupt(uint8_t vector, isr_t handler);
