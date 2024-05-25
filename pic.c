#include "pic.h"

void PIC_sendEOI(uint8_t irq){

   if(irq >= 8){
      outb(PIC2_COMMAND, PIC_EOI);
   }

   outb(PIC1_COMMAND, PIC_EOI);

}

void PIC_remap(int offset1, int offset2){

   uint8_t a1, a2; 

   a1 = inb(PIC1_DATA);
   a2 = inb(PIC2_DATA);

   outb(PIC1_COMMAND, 0x11);
   io_wait(); //boomer computers are a thing
   outb(PIC2_COMMAND, 0x11);
   io_wait();
   outb(PIC1_DATA, offset1);
   io_wait();
   outb(PIC2_DATA, offset2);
   io_wait();
   outb(PIC1_DATA, 4); //tell master it has a slave ready at irq 2
   io_wait();
   outb(PIC2_DATA, 2); //tell slave to be a slave
   io_wait();

   outb(PIC1_DATA, ICW4_8086);   //tell master pic mode
   io_wait();
   outb(PIC2_DATA, ICW4_8086); // tell slave pic mode
   io_wait();                              

   outb(PIC1_DATA, 0x01);
   io_wait();
   outb(PIC2_DATA, 0x0);

}

