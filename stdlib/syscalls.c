#include "syscalls.h"
#include <stdint.h>
#include "stdio.h"

uint8_t write(uint64_t fd, uint8_t* buf, uint64_t size){

   //syscall #1
   __asm__ volatile ("mov %0, %%rdi" : : "r" (fd) : "rdi");
   __asm__ volatile ("mov %0, %%rsi" : : "r" (buf) : "rsi");
   __asm__ volatile ("mov %0, %%rdx" : : "r" (size) : "rdx");
   __asm__ volatile ("mov $1, %%rax" : : : "rax");
   __asm__ volatile ("syscall");
}
uint64_t ret = -1;
uint8_t open(char* fileName, uint32_t flags, uint16_t mode){
   
   ret = -1;
   uint64_t fd = -1;

   //syscall #2
   __asm__ volatile ("mov %0, %%rdi" : : "r" (fileName) : "rdi");
   __asm__ volatile ("mov %0, %%esi" : : "r" (flags) : "rsi");
   __asm__ volatile ("mov %0, %%dx" : : "r" (mode) : "rdx");
   __asm__ volatile ("mov $2, %%rax" : : : "rax");
   __asm__ volatile ("syscall");
//   __asm__ volatile ("mov %%rax, ret(%%rip)" : : : );
   __asm__ volatile("mov %%rax, %0" : "=r" (fd) : :);
   
   return fd;

}

uint8_t poll(pollfd_t* fds, uint8_t nfds, uint16_t timeout){

   ret = -1;
   uint64_t scancode = -1;

   //syscall #7
   __asm__ volatile ("mov %0, %%rdi" : : "r" (fds) : "rdi");
   __asm__ volatile ("mov %0, %%sil" : : "r" (nfds) : "rsi");
   __asm__ volatile ("mov %0, %%dx" : : "r" (timeout) : "rdx");
   __asm__ volatile ("mov $7, %%rax" : : : "rax");
   __asm__ volatile ("syscall");
//   __asm__ volatile ("mov %%rax, ret(%%rip)" : : : );
   __asm__ volatile("mov %%rax, %0" : "=r" (scancode) : :);
   
   return (uint8_t)scancode;


}

uint8_t exit(uint32_t errorCode){

   __asm__ volatile ("mov %0, %%edi\n\t"
         "mov $60, %%rax\n\t"
         "syscall\n\t" : :"r"(errorCode) :);

}
