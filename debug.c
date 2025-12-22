#include <stdint.h>
#include "utils.h"
#define PORT 0x3f8

uint8_t initSerial(){

   outb(PORT +1, 0x00); //disable interupts
   outb(PORT +3, 0x80); //enable DLAB ? 
   outb(PORT +0, 0x03); 
   outb(PORT +1, 0x00);
   outb(PORT +3, 0x03);
   outb(PORT +2, 0xC7);
   outb(PORT +4, 0x0B);
   outb(PORT +4, 0x1E);
   outb(PORT +0, 0xAE);

   if(inb(PORT + 0) != 0xAE){

      return 1;

   }

   outb(PORT + 4, 0x0F);
   return 0;

}

int isTransmitEmpty(){

   return inb(PORT + 5) & 0x20;

}

void writeSerial(char a){

   while(isTransmitEmpty() == 0);

   outb(PORT, a);

}
