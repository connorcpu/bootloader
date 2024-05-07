#include "keyboard.h"
#include <stdint.h>
#include "utils.h"
#include <stdbool.h>

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

void keyboardHandler(registers_t r){

   uint8_t scancode = inb(0x60);

   switch (scancode) {
      case 0x0:
         print("ERROR");
         break;
      case 0x1:
         print("ESC");
         break;
      case 0x2:
         printch('1');
         break;
      case 0x3:
         printch('2');
         break;
      case 0x4:
         printch('3');
         break;
      case 0x5:
         printch('4');
         break;
      case 0x6:
         printch('5');
         break;
      case 0x7:
         printch('6');
         break;
      case 0x8:
         printch('7');
         break;
      case 0x9:
         printch('8');
         break;
      case 0x0A:
         printch('9');
         break;
      case 0x0B:
         printch('0');
         break;
      case 0x0C:
         printch('-');
         break;
      case 0x0D:
         printch('+');
         break;
      case 0x0E:
         backspace();
         break;
      case 0x0F:
         printch(0x20);
         printch(0x20);
         printch(0x20);
         break;
      case 0x10:
         printch('q');
         break;
      case 0x11:
         printch('w');
         break;
      case 0x12:
         printch('e');
         break;
      case 0x13:
         printch('r');
         break;
      case 0x14:
         printch('t');
         break;
      case 0x15:
         printch('y');
         break;
      case 0x16:
         printch('u');
         break;
      case 0x17:
         printch('i');
         break;
      case 0x18:
         printch('o');
         break;
      case 0x19:
         printch('p');
         break;
      case 0x1A:
         printch('[');
         break;
      case 0x1B:
         printch(']');
         break;
      case 0x1C:
         enter();
         break;
      case 0x1D:
         print("LCtrl");
         break;
      case 0x1E:
         printch('a');
         break;
      case 0x1F:
         printch('s');
         break;
      case 0x20:
         printch('d');
         break;
      case 0x21:
         printch('f');
         break;
      case 0x22:
         printch('g');
         break;
      case 0x23:
         printch('h');
         break;
      case 0x24:
         printch('j');
         break;
      case 0x25:
         printch('k');
         break;
      case 0x26:
         printch('l');
         break;
      case 0x27:
         printch(';');
         break;
      case 0x28:
         printch('\'');
         break;
      case 0x29:
         printch('`');
         break;
      case 0x2A:
         setShift(true);
         break;
      case 0x2B:
         print("\\");
         break;
      case 0x2C:
         printch('z');
         break;
      case 0x2D:
         printch('x');
         break;
      case 0x2E:
         printch('c');
         break;
      case 0x2F:
         printch('v');
         break;
      case 0x30:
         printch('b');
         break;
      case 0x31:
         printch('n');
         break;
      case 0x32:
         printch('m');
         break;
      case 0x33:
         printch(',');
         break;
      case 0x34:
         printch('.');
         break;
      case 0x35:
         printch('/');
         break;
      case 0x36:
         setShift(true);
         break;
      case 0x37:
         print("Keypad *");
         break;
      case 0x38:
         print("LAlt");
         break;
      case 0x39:
         printch(0x20);
         break;
      case 0x36 + 0x80:
         setShift(false);
         break;
      case 0x2A + 0x80:
         setShift(false);
         break;
      default:
         /* 'keuyp' event corresponds to the 'keydown' + 0x80 
          * it may still be a scancode we haven't implemented yet, or
          * maybe a control/escape sequence */
         if (scancode <= 0x7f) {
            print("Unknown key down");
         } else if (scancode <= 0x39 + 0x80) {
           // print("key up ");
         } else print("Unknown key up");
         break;
   }

}
