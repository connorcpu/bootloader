#include "utils.h"
#include <stdint.h>

void invlpg(void* addr){

   asm volatile("invlpg (%0)" :: "r"(addr) : "memory");

}

void load_cr3(void * cr3_value){

   asm volatile("mov %0, %%cr3" :: "r"((uint64_t)cr3_value): "memory");

}

void outb(uint16_t port, uint8_t val){

   //both of these work i dont know which is better
   //__asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
   __asm__("out %%al, %%dx" : : "a" (val), "d" (port));

   return;

}

uint8_t inb(uint16_t port){

   uint8_t ret;
   
   //both of these work i dont know which is better
/*   __asm__ volatile ( "inb %w1, %b0"
                     : "=a"(ret)
                     : "Nd"(port)
                     : "memory");*/
   __asm__("in %%dx, %%al" : "=a" (ret) : "d" (port));
   return ret;

}

void insl(int32_t port, void *addr, int32_t cnt){

  asm volatile("cld; rep insl" : "=D" (addr), "=c" (cnt) : "d" (port), "0" (addr), "1" (cnt) : "memory", "cc"); 

}

void io_wait(void){

   outb(0x80, 0);

}

int tolower(int character){

   if(character < 91 && character > 0x40) return character += 0x20;

   return character;

}

int toupper(int character){

   if(character < 0x7b && character > 0x60) return character -= 0x20;

   return character;

}

int strcmp (const char* str1, const char* str2){

   while(*str1 && (*str1 == *str2)){
        str1++;
        str2++;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2;

}

void memcpy(void *src, void *dest, uint32_t size){

   uint8_t* csrc = (uint8_t*)src;
   uint8_t* cdest = (uint8_t*)dest;

   for (int i = 0; i < size; i++) {

      cdest[i] = csrc[i];
   
   }

}
