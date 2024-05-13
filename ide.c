#include "utils.h"
#include "ide.h"
#include <stdint.h>

uint8_t ide_buf[2048] = {0};
volatile unsigned static char ide_irq_invoked = 0;
unsigned static char atapi_packet[12] = {0xA8, 0,0,0,0,0,0,0,0,0,0,0};

struct IDEChannelRegisters {

   uint16_t base; //io base 
   uint16_t ctrl; //control base 
   uint16_t bmide; //bus master ide
   uint8_t nIEN; //nIEN (no interupt)

} channels[2];

struct ide_device {

   uint8_t reserved; //0 (empty) || 1 (drive is real)
   uint8_t channel; //0 (primairy) || 1 (secondary)
   uint8_t drive; //0 (master) || 1 (slave)
   uint16_t type; //0 (ata) || 1 (atapi)
   uint16_t signature; 
   uint16_t capabilities;
   uint32_t commandSets;
   uint32_t size; 
   uint8_t model[41];

} ide_devices[4];

uint8_t ide_read(uint8_t channel, uint8_t reg){

   uint8_t result;

   if(reg > 0x07 && reg < 0x0c)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   if(reg < 0x08)
      result = inb(channels[channel].base + reg - 0x00);
   else if(reg < 0x0c)
      result = inb(channels[channel].base + reg - 0x06);
   else if(reg < 0x0e)
      result = inb(channels[channel].ctrl + reg - 0x0a);
   else if(reg < 0x16)
      result = inb(channels[channel].bmide + reg - 0x0e);
   if(reg > 0x07 && reg < 0x0c)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
   return result;

}

void ide_write(uint8_t channel, uint8_t reg, uint8_t data){

   if(reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   if (reg < 0x08)
      outb(channels[channel].base + reg - 0x00, data);
   else if(reg < 0x0c)
      outb(channels[channel].base + reg - 0x06, data);
   else if(reg < 0x0e)
      outb(channels[channel].ctrl + reg - 0x0a, data);
   else if(reg < 0x16)
      outb(channels[channel].bmide + reg - 0x0e, data);

}

void ide_read_buffer(uint8_t channel, uint8_t reg, uint32_t buffer, uint32_t quads){

   //TODO current code trashes ES & ESP between assembly blocks, fix
   if (reg > 0x07 && reg < 0x0c)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   //asm("pushw %es; movw %ds, %ax; movw %ax, %es");
   if (reg < 0x08) 
      insl(channels[channel].base + reg - 0x00, buffer, quads);
   else if (reg < 0x0c) 
      insl(channels[channel].base + reg - 0x06, buffer, quads);
   else if (reg < 0x0e) 
      insl(channels[channel].ctrl + reg - 0x0a, buffer, quads);
   else if (reg < 0x16) 
      insl(channels[channel].bmide + reg - 0x0e, buffer, quads);
   //asm("popw %es;");
   if(reg > 0x07 && reg < 0x0c)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);

}

void ideInit(uint32_t bar0, uint32_t bar1, uint32_t bar2, uint32_t bar3, uint32_t bar4){

   return;

}
