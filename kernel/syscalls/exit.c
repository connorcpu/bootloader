#include "exit.h"

uint8_t sysExit(uint32_t error){

   kprintf("process exited with code %i\n", error);
   __asm__ volatile ("xchg %bx, %bx");
   return 0;

}
