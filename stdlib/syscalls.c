#include "syscalls.h"
#include <stdint.h>

uint8_t write(uint64_t fd, uint8_t* buf, uint64_t size){

   //syscall #1
   __asm__ volatile ("mov $1, %rax");
   __asm__ volatile ("mov %0, %%rdi" : : "r" (fd));
   __asm__ volatile ("mov %0, %%rsi" : : "r" (buf));
   __asm__ volatile ("mov %0, %%rdx" : : "r" (size));

}
