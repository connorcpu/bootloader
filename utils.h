#include <stdint.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void io_wait(void);

typedef struct {

   uint64_t ds;
   uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
   uint64_t int_no, err_code;
   uint64_t rip, cs, eflags, userrsp, ss;

} registers_t;
