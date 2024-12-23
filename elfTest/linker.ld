ENTRY(main)
OUTPUT_FORMAT("binary")

MEMORY{
   ROM (rx) : ORIGIN = 0x10400, LENGTH = 4k
   RAM (wx) : org = 0x11400, len = 4k
}

SECTIONS 
{
   . = 0x10400;
   .text : AT(0x10400){ 
      . = ALIGN(4);
      *(.text) 
      *(.text*) 
   }

   .data : {
      *(.data) 
      *(.data*)
   }

   .bss : { *(.bss) }
   .rodata : { *(.rodata) }

}
