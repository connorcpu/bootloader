#include "fat.h"
uint8_t executeElf(fileHeader_t* file);
void executeRaw(fileHeader_t* file);

typedef struct elfHeader{

   uint32_t magic;
   uint8_t bits; //1=32bit 2=64
   uint8_t endian; //1 = litle, 2=big 
   uint8_t elfHeaderVersion;
   uint8_t OSABI; //0 for System V
   uint64_t unused; //padding
   uint16_t type; //1=relocatable, 2=executable, 3=shared, 4/core
   uint16_t instructionSet; //0x03 = x86, 0x03E = x86-64
   uint32_t elfVersion;
   uint64_t programEntryOffset;
   uint64_t programHeaderOffset;
   uint64_t sectionHeaderOffset;
   uint32_t flags; //architechture dependent
   uint16_t headerSize;
   uint16_t programHeaderEntrySize;
   uint16_t programHeaderEntryCount;
   uint16_t sectionHeaderEntrySize;
   uint16_t sectionHeaderEntryCount;
   uint16_t stringTableIndex; //??

}__attribute__((packed)) elf64Header_t;


typedef struct programHeaderTableEntry{ 

   uint32_t type; //0=ignore, 1=load, there is more but not relevant
   uint32_t flags;
   uint64_t p_offset; //offset from file start where this segments data can be found;
   uint64_t p_vaddr; //where in memory this segment should be loaded
   uint64_t p_paddr; //reserved for segment's physical address
   uint64_t p_filesz; //size of the segment in file                     
   uint64_t p_memsz; //size of segment in mem, greater or equal to p_filesz;
   uint64_t alignment; //usually a power of 2
   
}__attribute__((packed)) programHeaderEntry_t; 
