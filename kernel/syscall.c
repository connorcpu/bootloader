#include "syscall.h"

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
   //Set LSTAR to the RIP value for the syscall entry point (since higherhalf kernel always the same)
   //set STAR 47:32 to KERNEL CS 
   //set SFMASK if relavent (think just leave it at 0x00000)
   //chatGPT recommends setting SFMASK to 0x200 to clear the Interupt flag

}

void handleSyscall(){

   //entrypoint for syscall 
   //
   //what this method should do:
   //switch to kernel stack
   //store ECX for returning later
   //handle syscall according to yet undetermined syscall table
   //setup and execute iretq, later try sysret

}
