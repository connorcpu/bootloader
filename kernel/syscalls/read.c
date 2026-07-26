#include <stdint.h>
#include "read.h"
#include "../syscall.h"
#include "../io.h"

size_t sysRead(uint64_t rdi, uint64_t rsi, uint64_t rdx){

   if(rdi == 0){

      ((char*)rsi)[0] = getScancode();

   }
   
   return 1;

}

