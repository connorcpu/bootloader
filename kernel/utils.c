#include "io.h"
#include "utils.h"

void invlpg(void* addr){

   asm volatile("invlpg (%0)" :: "r"(addr) : "memory");

}

void load_cr3(void * cr3_value){

   asm volatile("mov %0, %%cr3" :: "r"((uint64_t)cr3_value): "memory");

}

void outb(uint16_t port, uint8_t val){
   uint16_t p = port;
   //both of these work i dont know which is better
   //__asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
   __asm__("push %rax");
   __asm__("out %%al, %%dx" : : "a" (val), "d" (p));
   __asm__("pop %rax");

   return;

}

void outl(uint16_t port, uint32_t val){

   uint16_t p = port;
   __asm__ volatile("out %0, %1\n\t" : : "r" (val), "r" (p) : );

}

uint32_t inl(uint16_t port){

   uint32_t ret;
   uint16_t p = port;
   __asm__("in %%edx, %%eax" : "=a" (ret) : "d" (p));
   return ret;

}

uint8_t inb(uint16_t port){

   uint8_t ret;
   uint16_t p = port;

   //both of these work i dont know which is better
/*   __asm__ volatile ( "inb %w1, %b0"
                     : "=a"(ret)
                     : "Nd"(port)
                     : "memory");*/
   __asm__("in %%dx, %%al" : "=a" (ret) : "d" (p));
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
