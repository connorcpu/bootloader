#include "syscall.h"
#include "io.h"
#include "syscalls/read.h"
#include "syscalls/write.h"
#include "debug.h"

void setupSyscall(){

   //STAR = 0xc0000081
   //LSTAR = 0xc0000082
   //CSTAR = 0xc0000083
   //SFMASK = 0xc0000084
   //RIP from LSTAR
   //RFLAGS from R11
   //kernel CS from STAR 47:32
   //kernel SS from STAR 47:32 + 8
   //userland CS from STAR 63:48 + 16 on sysret
   //userland SS from STAR 63:48 + 8 on sysret
   //if using sysret instead of iretq set operand size to 64 bits or you will enter compatibility mode ("064 sysret")
   //RIP is stored into ECX and loaded from there on sysret
   //
   //
   //what this method should do:
   //1. Set LSTAR to the RIP value for the syscall entry point (since higherhalf kernel always the same)
   //2. set STAR 47:32 to KERNEL CS 
   //3. set SFMASK if relavent (think just leave it at 0x00000)
   //chatGPT recommends setting SFMASK to 0x200 to clear the Interupt flag
   
   //1. set LSTAR to entry point address
/*   __asm__ volatile ("mov $0xC0000082, %%ecx" : : : "ecx"); //ECX contains which MSR we'll be writing to
   __asm__ volatile ("movq %0, %%rax" : : "r" (&handleSyscall) : "rax"); //eax contains what we'll be writing to the MSR
   __asm__ volatile ("wrmsr");*/

   uint64_t addr = (uint64_t)&handleSyscall;

   __asm__ volatile (
       "mov $0xC0000082, %%ecx\n\t"
       "mov %0, %%rax\n\t"
       "shr $32, %%rax\n\t"
       "mov %%eax, %%edx\n\t"   // high 32 bits
       "mov %0, %%rax\n\t"      // reload full value
       "wrmsr"
       :
       : "r"(addr)
       : "rax", "rdx", "rcx"
   );

   //2. set Star 47:32 to 0x08
   __asm__ volatile ("mov $0xc0000081, %%ecx" : : : "ecx");
   __asm__ volatile ("rdmsr" : : : "rax");
   unsigned long val = 0x08UL << 32;
   __asm__ volatile ("or %0, %%rax" : : "r"(val) : "rax"); //intermedate values cannot be 64 bits wide so we need to go through a register
   __asm__ volatile ("wrmsr");

   //3. set SFMask 
   __asm__ volatile ("mov $0xc0000084, %%ecx" : : : "ecx");
   __asm__ volatile ("mov $0x200, %%eax" : : : "eax");
   __asm__ volatile ("wrmsr");

}

void handleSyscall(){

   //entrypoint for syscall 
   //
/*   uint64_t length;
   uint8_t* ptr;

   __asm__ volatile("mov %%rsi, %0" : "=r" (ptr));
   __asm__ volatile("mov %%rdx, %0" : "=r"(length));
   bochsBreak();*/

   //kprintf("ptr: %i\n", ptr);
  // kprintf("len: %i\n", length);

   //ptr[length] = 0x00;
   //kprintf("%s", ptr);
   
   uint64_t syscallNr = -1;
   uint64_t rdi = -1;
   uint64_t rsi = -1;
   uint64_t rdx = -1;
   uint64_t r10 = -1;
   uint64_t r8 = -1;
   uint64_t r9 = -1;
   __asm__ volatile("mov %%rax, %0" : "=r" (syscallNr));
   __asm__ volatile("mov %%rdi, %0" : "=r" (rdi));
   __asm__ volatile("mov %%rsi, %0" : "=r" (rsi));
   __asm__ volatile("mov %%rdx, %0" : "=r" (rdx));
   __asm__ volatile("mov %%r10, %0" : "=r" (r10));
   __asm__ volatile("mov %%r8, %0" : "=r" (r8));
   __asm__ volatile("mov %%r9, %0" : "=r" (r9));

   kprintf("registered syscall number: %d\n", syscallNr);
   
   switch(syscallNr){

      case 0x00:
         sysRead(rdi, rsi, rdx);
         break;
      case 0x01:
         sysWrite(rdi, rsi, rdx);
         break;

      default: 
         kprintf("syscall number %h not found \n", syscallNr);
         break;

   }


   //what this method should do:
   //switch to kernel stack
   //store ECX for returning later
   //handle syscall according to yet undetermined syscall table
   //setup and execute iretq, later try sysret

}

