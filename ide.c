#include "utils.h"
#include "ide.h"
#include <stdint.h>
#include "io.h"

uint8_t ide_buf[2048] = {0};
volatile unsigned static char ide_irq_invoked = 0;
unsigned static char atapi_packet[12] = {0xA8, 0,0,0,0,0,0,0,0,0,0,0};
unsigned static char package[1] = {0};

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
      insl(channels[channel].base + reg - 0x00, (void *)buffer, quads);
   else if (reg < 0x0c) 
      insl(channels[channel].base + reg - 0x06, (void *)buffer, quads);
   else if (reg < 0x0e) 
      insl(channels[channel].ctrl + reg - 0x0a, (void *)buffer, quads);
   else if (reg < 0x16) 
      insl(channels[channel].bmide + reg - 0x0e, (void *)buffer, quads);
   //asm("popw %es;");
   if(reg > 0x07 && reg < 0x0c)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);

}

uint8_t ide_polling(uint8_t channel, uint32_t advanced_check){

   //wait 400 nanoseconds, reading ATA_REG_ALTSTATUS wastes 100
   for (int i = 0; i < 4; i++) {
      ide_read(channel, ATA_REG_ALTSTATUS);
   }

   while(ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
      ; //wait for it to be ready
   
   if(advanced_check){

      uint8_t state = ide_read(channel, ATA_REG_STATUS);

      //error checking
      if (state & ATA_SR_ERR) 
         return 2; //error
      if (state & ATA_SR_DF)
         return 1; //device fault
      if ((state & ATA_SR_DRQ) == 0)
         return 3; //DRQ should be set
      
   }

   return 0; //no error

}

uint8_t ide_print_error(uint32_t drive, uint8_t err){

   if(err == 0)
      return err;
   
   kprintf("ERROR ON DRIVE %d\n", drive);
   kprintf("ERROR NUMBER: %d\n", err);
   if(err == 2){
      uint8_t st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
      putch(st + '0', 4, 0);
   }

   return err;

}

void ideInit(uint32_t bar0, uint32_t bar1, uint32_t bar2, uint32_t bar3, uint32_t bar4){

   print("we are doing smt");

   int i, j, k, count = 0;

   //detect io ports
   channels[ATA_PRIMARY].base = (bar0 & 0xFFFFFFFC) + 0x1F0 * (!bar0);
   channels[ATA_PRIMARY].ctrl = (bar1 & 0xFFFFFFFC) + 0x3F6 * (!bar1);
   channels[ATA_SECONDARY].base = (bar2 & 0xFFFFFFFC) + 0x170 * (!bar2);
   channels[ATA_SECONDARY].ctrl = (bar3 & 0xFFFFFFFC) + 0x376 * (!bar3);
   channels[ATA_PRIMARY].bmide = (bar4 & 0xFFFFFFFC) + 0;
   channels[ATA_SECONDARY].bmide = (bar4 & 0xFFFFFFFC) + 8;

   //disable irqs
   ide_write(ATA_PRIMARY, ATA_REG_CONTROL, 2);
   ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

   //detect ATA-ATAPI devices
   for (i = 0; i < 2; i++) {
      for (j = 0; j < 2; j++) {

         uint8_t err = 0, type = IDE_ATA, status;
         ide_devices[count].reserved = 0;

         //select drive
         ide_write(i, ATA_REG_HDDEVSEL, 0xa0 | (j << 4));
        // sleep(1); //unimplemented
         
         //be smart, dont do this
         for (int l = 0; l < 10; l++) {
            ide_read(1, ATA_REG_ALTSTATUS);
         }

         //send ATA identify command 
         ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
         // sleep(1); 
         // replace ASAP
         for (int l = 0; l < 10; l++) {
            ide_read(1, ATA_REG_ALTSTATUS);
         }

         //polling 
         if (ide_read(i, ATA_REG_STATUS) == 0) continue;       

         while (1) {

//            print("while 1'ing");

            status = ide_read(i, ATA_REG_STATUS);
            if((status & ATA_SR_ERR)) {err = 1; break;} //device is not ATA 
            if(!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break; //we ist good
         
         }

         //probe for atapi devices 
         if(err !=0){

            uint8_t cl = ide_read(i, ATA_REG_LBA1);
            uint8_t ch = ide_read(i, ATA_REG_LBA2);

            if (cl == 0x14 && ch == 0xEB) {
               type = IDE_ATAPI;
            }else if (cl = 0x69 && ch == 0x96) {
               type = IDE_ATAPI;
            }else {
               continue;
            }
            
            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
            //sleep(1);

         }

         //read identify space
         ide_read_buffer(i, ATA_REG_DATA, (uint32_t)ide_buf, 128);

         //read device params
         ide_devices[count].reserved = 1;
         ide_devices[count].type = type;
         ide_devices[count].channel = i;
         ide_devices[count].drive = j;
         ide_devices[count].signature = *((uint16_t *)(ide_buf + ATA_IDENT_DEVICETYPE));
         ide_devices[count].capabilities = *((uint16_t *)(ide_buf + ATA_IDENT_CAPABILITIES));
         ide_devices[count].commandSets = *((uint32_t *)(ide_buf + ATA_IDENT_COMMANDSETS));

         //get size
         if(ide_devices[count].commandSets & (1 << 26))
            //LBA48
            ide_devices[count].size = *((uint32_t *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
         else 
            //CHS or 28bit addressing
            ide_devices[count].size = *((uint32_t *)(ide_buf + ATA_IDENT_MAX_LBA));

         //get model 
         for(k = 0; k < 40; k += 2){

            ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
            ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];

         }

         ide_devices[count].model[40] = 0; //terminate string
         count++;

      }
   }

   //print summary
   for(i = 0; i < 4; i++){

      if(ide_devices[i].reserved == 1){

         kprintf("found drive of type: %s\n", (char *[]){"ATA", "ATAPI"}[ide_devices[i].type]);
         kprintf("size: %d\n", (ide_devices[i].size));
         kprintf("model: %s\n", ide_devices[i].model);

         

      }

   }
   
}


uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint32_t lba, uint8_t numSects, uint16_t selector, uint32_t edi){

   uint8_t lba_mode /* 0=chs, 1=lba28, 2=lba48 */, dma, cmd;
   uint8_t lba_io[6];
   uint32_t channel = ide_devices[drive].channel;
   uint32_t slavebit = ide_devices[drive].drive;
   uint32_t bus = channels[channel].base;
   uint32_t words = 256;
   uint16_t cyl, i;
   uint8_t head, sect, err;

   ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);

   if(lba >= 0x10000000){

      lba_mode = 2; //lba48
      lba_io[0] = (lba & 0x000000FF) >> 0;
      lba_io[1] = (lba & 0x0000FF00) >> 8;
      lba_io[2] = (lba & 0x00FF0000) >> 16;
      lba_io[3] = (lba & 0xFF000000) >> 24;
      lba_io[4] = 0; //not needed in lba48
      lba_io[5] = 0;
      head      = 0;

   }else if (ide_devices[drive].capabilities & 0x200) { //drive supports lba?

                                                
      lba_mode = 1; //lba28
      lba_io[0] = (lba & 0x00000FF) >> 0;
      lba_io[1] = (lba & 0x000FF00) >> 8;
      lba_io[2] = (lba & 0x0FF0000) >> 16;
      lba_io[3] = 0;
      lba_io[4] = 0; //not needed in lba48
      lba_io[5] = 0;
      head      = (lba & 0xF000000) >> 24;
   
   }else { //CHS

      lba_mode = 0; //chs
      sect = (lba % 63) + 1;
      cyl = (lba + 1 -sect) /(16 * 63);
      lba_io[0] = (lba & 0x00000FF) >> 0;
      lba_io[1] = (lba & 0x000FF00) >> 8;
      lba_io[2] = (lba & 0x0FF0000) >> 16;
      lba_io[3] = 0;
      lba_io[4] = 0; //not needed in lba48
      lba_io[5] = 0;
      head      = (lba + 1 - sect) % (16* 63) / (63);
   
   }

   dma = 0;

   while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY) {
   
   } //we ist busy, shush tf up

   //select drive
   if(lba_mode == 0)
      ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); //drive & chs 
   else
    ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); //drive & LBA

   //write params
   if(lba_mode == 2){
   
      ide_write(channel, ATA_REG_SECCOUNT1, 0);
      ide_write(channel, ATA_REG_LBA3, lba_io[3]);
      ide_write(channel, ATA_REG_LBA4, lba_io[4]);
      ide_write(channel, ATA_REG_LBA5, lba_io[5]);

   }

   ide_write(channel, ATA_REG_SECCOUNT0, numSects);
   ide_write(channel, ATA_REG_LBA0, lba_io[0]);
   ide_write(channel, ATA_REG_LBA1, lba_io[1]);
   ide_write(channel, ATA_REG_LBA2, lba_io[2]);

   //choose the command
   if (lba_mode == 0 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
   if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;   
   if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;   
   if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
   if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
   if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
   if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
   if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
   if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
   if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
   if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
   if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;

   ide_write(channel, ATA_REG_COMMAND, cmd);               // Send the Command.

   if (dma)
      if (direction == 0);
         //dma read code (fuck it)
      else;
         //dma write code (kill yourself)
   else
      if(direction == 0)
         //pio read 
      for(i = 0; i < numSects; i++){

         if(err = ide_polling(channel, 1))
            return err;
         __asm__ volatile("pushw %bx");
         __asm__ volatile("mov %es, %bx");
         __asm__ volatile("mov %%ax, %%es" : : "a"(selector));
         __asm__ volatile("rep insw" : : "c"(words), "d"(bus), "D"(edi)); //recieve data
         __asm__ volatile("mov %bx, %es");
         __asm__ volatile("popw %bx");
         edi += (words*2);
      }else{
         //pio write
         for(i = 0; i < numSects; i++){

            ide_polling(channel, 0);
            __asm__ volatile("pushw %bx");
            __asm__ volatile("mov %ds, %bx");
            __asm__ volatile("mov %%ax, %%ds" :: "a"(selector));
            __asm__ volatile("rep outsw" :: "c"(words), "d"(bus), "S"(edi)); //send data
            __asm__ volatile("mov %bx, %ds");
            __asm__ volatile("popw %bx");
            edi += (words*2);

         }
         ide_write(channel, ATA_REG_COMMAND, (char []){ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
         ide_polling(channel, 0);

      }


   return 0;

}

uint8_t ide_read_sectors(uint8_t drive, uint8_t numssects, uint32_t lba, uint16_t es, uint32_t edi){

   //check if drive is real 
   if (drive > 3 || ide_devices[drive].reserved == 0) package[0] = 0x1; //drive not found
                                                                     
   //check func inputs
   else if (((lba + numssects) > ide_devices[drive].size) && (ide_devices[drive].type == IDE_ATA))
      package[0] = 0x2; //invalid inputs

   else{
   
      uint8_t err;
      if (ide_devices[drive].type == IDE_ATA) {
         err = ide_ata_access(ATA_READ, drive, lba, numssects, es, edi);
      }else if (ide_devices[drive].type == IDE_ATAPI) {
         kprintf("ERROR: IDE_ATAPI READING NOT IMPLEMENTED");
      }
      package[0] = ide_print_error(drive, err);

   }
  
   return package[0];

}
