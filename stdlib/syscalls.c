#include "syscalls.h"
#include <stdint.h>

uint8_t write(uint64_t fd, uint8_t* buf, uint64_t size){

   //syscall #1
   __asm__ volatile ("mov %0, %%rdi" : : "r" (fd) : "rdi");
   __asm__ volatile ("mov %0, %%rsi" : : "r" (buf) : "rsi");
   __asm__ volatile ("mov %0, %%rdx" : : "r" (size) : "rdx");
   __asm__ volatile ("mov $1, %%rax" : : : "rax");
   __asm__ volatile ("syscall");
   __asm__ volatile("popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         : : : "rax");

   /*__asm__ volatile ("mov %0, %%rdi"
                     "mov %1, %%rsi"
                     "mov %2, %%rdx"
                     "syscall" : : "r"(fd), "r"(buf), "r"(size));*/
}

uint8_t open(char* fileName, uint32_t flags, uint16_t mode){

   __asm__ volatile ("mov %0, %%rdi" : : "r" (fileName) : "rdi");
   __asm__ volatile ("mov %0, %%esi" : : "r" (flags) : "rsi");
   __asm__ volatile ("mov %0, %%dx" : : "r" (mode) : "rdx");
   __asm__ volatile ("mov $2, %%rax" : : : "rax");
   __asm__ volatile ("syscall");

   __asm__ volatile("popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         "popq %%rax\n\t"
         : : : "rax");

}
