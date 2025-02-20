ENTRY(_start)
OUTPUT_FORMAT(binary)

MEMORY{
   RAM (rwx) : org = 0x0, len = 4k
}

SECTIONS 
{
   . = 0x0;
   .text : AT(0x00){ 
      *(.text) 
      *(.text*) 
   }

   .data : {
      *(.data) 
      *(.data*)
   }

   .bss : { *(.bss) }
   .rodata : { *(.rodata) }

   /DISCARD/ : {
      *(.comment)
   }

}
