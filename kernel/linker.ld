/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

STARTUP(bin/main.o)
OUTPUT(bin/main.bin)

/*OUTPUT_FORMAT(binary)*/
OUTPUT_FORMAT(binary)

SECTIONS {

   .text 0xc0000000 : { *(.text)}

}

