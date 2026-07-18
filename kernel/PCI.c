#include "PCI.h"
#include "io.h"
#include "utils.h"


uint16_t pciRead(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset){

   //basically all you need to do is assemle the data into 1 data dword, then put it into the CONFIG_ADDRESS (0xCF8)
   //then just read in the CONFIG_DATA (0xCFC)
   //now when you read it in you read in the whole register at a time
   //note that the control word can also be assembled using bitshifting

/*   pciConfig_t addr;
   addr.enable = 1; //not sure what this does, something about translating CONFIG_DATA accesses to config cycles
   addr.busNum = bus;
   addr.devNum = dev;
   addr.funcNum = func;
   addr.regOffset = offset;*/

   //offsett is & with 0xFC because it must be DWORD aligned, the later offset correction is able to handle unaligned calls
   //for byte allignment you still have to muck about yourself
   uint32_t addr = (bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC) | ((uint32_t)(0x1 << 31));

   outl(CONFIG_ADDRESS, (uint32_t)addr);

   //not sure but it's recommended to also manually reapply bitshifting the offset?
   //i think this is because the result could include the next offset's too
   uint32_t raw = inl(CONFIG_DATA);

   return (uint16_t)((raw >> ((offset & 2) * 8)) & 0xFFFF);

}

void pciDetectAll(){

   uint16_t vendor, device, tmp1, tmp2;
   uint8_t class, subclass, progIF, headerType;

   //so for every bus we try every dev number
   //we trigger func 0 on this thing, seems to return identity info as well as triggering the devices "primary" funcion
   //then if it returns 0xFFFF it did not find a device
   //if it did not return 0xFFFF we also check dev ID
   
   //for(uint8_t i = 0; i < 256; i++){
   for(uint8_t i = 0; i < 6; i++){

      for(uint8_t j = 0; j < 16; j++){

         if((vendor = pciRead(i, j, 0, 0)) != 0xFFFF){

            device = pciRead(i, j, 0, 0x2);

            uint16_t tmp = pciRead(i, j, 0, 0x8);
            progIF = (uint8_t)(tmp >> 8) & 0xFF;

            tmp2 = pciRead(i, j, 0, 10);
            subclass = (uint8_t)(tmp2 & 0xFF);
            class = (uint8_t)((tmp2 >> 8) & 0xFF);

            headerType = (uint8_t)(pciRead(i, j, 0, 14) & 0xFF);

            kprintf("vendor: %h, dev: %h, class: %h, subclass: %h, progIF: %h, headerType: %h\n"
                  , vendor, device, class, subclass, progIF, headerType);
         }
         
      }
   }
}
