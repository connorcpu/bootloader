
#include <stdint.h>
void* putPixel(uint16_t x, uint16_t y, uint32_t colour){

   asm volatile(
         "mov $2, %%rax\n"
         "syscall\n"
         : 
         : "b" (x),
           "c" (y),
           "d" (colour)
         
         );

}
