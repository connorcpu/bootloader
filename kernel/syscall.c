#include "syscall.h"
#include "io.h"
#include "syscalls/read.h"
#include "syscalls/write.h"
#include "syscalls/open.h"
#include "debug.h"

uint64_t VBEIBA;

uint64_t getVBEIBA(){

   return VBEIBA;

}

void setupSyscall(uint64_t VBEInfoBlockAddr){

   //btw also handle the VBEInfo block for now
   VBEIBA = VBEInfoBlockAddr;

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
   //2.5 set STAR 64:48 to user CS
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
   //so turns out the MSR is actually red from EAX and EDX.....

   uint64_t star = (0x08UL << 32) | (0x08UL << 48);
   uint32_t low = (uint32_t) star;
   uint32_t high = (uint32_t) (star >> 32);

   //2.5, set star 63:48 to 0x18;
   __asm__ volatile("mov $0xc0000081, %%ecx\n\t"
                     "mov %0, %%eax\n\t"
                     "mov %1, %%edx\n\t"
                     "wrmsr"
                     :
                     : "r" (low), "r"(high)
                     : "rax", "rdx", "rcx"
                     );

   //3. set SFMask 
   __asm__ volatile ("mov $0xc0000084, %%ecx" : : : "ecx");
   __asm__ volatile ("mov $0x200, %%eax" : : : "eax");
   __asm__ volatile ("wrmsr");

}

uint64_t saved_rcx = 0x00;
uint64_t saved_rax = -1;
uint64_t saved_rdi = -1;
uint64_t saved_rsi = -1;
uint64_t saved_rdx = -1;
uint64_t saved_r10 = -1;
uint64_t saved_r8 = -1;
uint64_t saved_r9 = -1;

__attribute__((naked))void handleSyscall(){

   //entrypoint for syscall 
   
   //save ECX
   
  /* __asm__ volatile("mov %%ecx, %0" : "=r" (ecx));
   __asm__ volatile("mov %%rax, %0" : "=r" (syscallNr));
   __asm__ volatile("mov %%rdi, %0" : "=r" (rdi));
   __asm__ volatile("mov %%rsi, %0" : "=r" (rsi));
   __asm__ volatile("mov %%rdx, %0" : "=r" (rdx));
   __asm__ volatile("mov %%r10, %0" : "=r" (r10));
   __asm__ volatile("mov %%r8, %0" : "=r" (r8));
   __asm__ volatile("mov %%r9, %0" : "=r" (r9));*/
   __asm__ volatile(
       "mov %%rcx, saved_rcx(%%rip)\n\t"
       "mov %%rax, saved_rax(%%rip)\n\t"
       "mov %%rdi, saved_rdi(%%rip)\n\t"
       "mov %%rsi, saved_rsi(%%rip)\n\t"
       "mov %%rdx, saved_rdx(%%rip)\n\t"
       "mov %%r10, saved_r10(%%rip)\n\t"
       "mov %%r8,  saved_r8(%%rip)\n\t"
       "mov %%r9,  saved_r9(%%rip)\n\t"
       :::
   );
/*   __asm__ volatile("mov %%rcx, %0\n\t"
                  "mov %%rax, %1\n\t"
                  "mov %%rdi, %2\n\t"
                  "mov %%rsi, %3\n\t"
                  "mov %%rdx, %4\n\t"
                  "mov %%r10, %5\n\t"
                  "mov %%r8,  %6\n\t"
                  "mov %%r9,  %7\n\t"
                  : "=m" (ecx), "=m"(syscallNr), "=m"(rdi), "=m"(rsi), "=m"(rdx), "=m"(r10), "=m"(r8), "=m"(r9));
*/
   kprintf("registered syscall number: %d, return addr: %h\n", saved_rax, saved_rcx);
   
   switch(saved_rax){

      case 0x00:
         sysRead(saved_rdi, saved_rsi, saved_rdx);
         break;
      case 0x01:
         sysWrite(saved_rdi, saved_rsi, saved_rdx);
         break;
      case 0x02:
         sysOpen((char*)saved_rdi, saved_rsi, saved_rdx);
         break;

      default: 
         kprintf("syscall number %h not found \n", saved_rax);
         break;

   }

   bochsBreak();

   //return uing sysret back to program
   /*__asm__ volatile("mov %0, %%ecx\n\t"
         "sysretq" 
         : 
         : "r"(ecx) : "ecx");*/

   __asm__ volatile(
         "mov $0x10, %%ax\n\t"
         "mov %%ax, %%ds\n\t"
         "mov %%ax, %%es\n\t"
         "mov %%ax, %%gs\n\t"
         "mov %%ax, %%fs\n\t"

         "mov %%rsp, %%rax\n\t"
         "pushq $0x10\n\t"
         "pushq %%rax\n\t"
         "pushfq\n\t"
         "pushq $0x08\n\t"
         "pushq %0\n\t"
         "iretq\n\t"
         :
         : "r"(saved_rcx)
         : "rax", "memory"
         );

   //what this method should do:
   //switch to kernel stack
   //store ECX for returning later
   //handle syscall according to yet undetermined syscall table
   //setup and execute iretq, later try sysret

}

