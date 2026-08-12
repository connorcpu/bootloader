# About
This is a personal passion project of mine that has been in semi-active development for several years now. In theory this code should constitute an Operating System.
The OS is 64-bit, runs in the higher-half, uses paging and has it's own bootloader for x86 MBR bios. 
The code in the main folder is the bootloader. The code in the kernel folder is the kernel and the main part of the OS.
The elfTest folder contains executable experimental code to be run by the OS.
The stdlib folder contains library code for any external executable to link against, in essence they are just syscall wrappers.
The osfs folder is copied into the os root file system on compile by the makefile.
The kernel/syscalls folder contains 1 file per syscall to handle that specific syscall.

# Features
- 64-bit
- higher-half
- paging
- (simple) elf execution
- syscall handling
- interrupt handling

# architecture
## for executables
On an executable level the OS tries to follow linux implementation: IO is handled by opening files and then reading/writing to the returned file-descriptor.
File-descriptors are currently implemented as nothing more than an index which returns a pointer.
User input is not quite yet ready to be handled the linux way. The framebuffer however can be written to by calling open("/dev/fb"); and then using write();.
For the most up-to-date information on available options, refer to the stdlib, assume functions behave (roughly) like their linux counterpart.
Alternativly, consult the kernel/syscalls file for implementation details of a specific syscall.
to compile an executable for this system you will probably want to use a cross-compiler and you will need to link against the provided std lib.
No start files are available as of yet. To add the resulting executable to the OS filesystem, output it to the osfs folder.
Example:
```bash
x86_64-elf-gcc test.c -fPIC -L stdlib/bin -static -lautism -nostdlib -nostartfiles -ffreestanding --entry=_start -o osfs/test.elf
```


## for people hoping to learn something
The OS is designed following the philosophy "never prepare for the future" and the idea to never return to 16-bits for bios calls once in 32-bits.
Especesially the lower bootloader suffered inmensely from me being to stuborn and lazy to redo parts of the code.
I also decided that I decidedly do _not_ want to write a fat driver in the first MBR sector. 
This means that instead the second stage of the bootloader is put on the disk directly following the first stage code, it is also loaded into memory as such. 

Loading the second (and third) stage is the only job of the first stage (boot.asm). 
The second stage consists of the assembly required to bring execution up to 64-bit C-code execution (A20, GDT, E820 but also VGA/VESA-VBE).
The third stage consists of the C code required to load the kernel from disk, into mapped memory.   

Because the second stage 'generates' filled data structures like the VBEInfoBlock, which contains vital framebuffer information; the second stage and third stage are linked together as one.
This is also why the interrupts stubs are in the stage 2 file, this is merely to save 1 file and seperatly adding it to the Makefile.
This architecture makes pasing structures like the E820 or VGA date trivially simple.  

This does however mean that both the second and third stage are on disk, in compiled form, occupying whatever space it needs from the second sector onwards, as seen in disk.asm, this is roughly (but less than) 25KiB.
Due to the aformentioned design philosophies and the fact that the second and third stage were compiled and loaded like this before FAT execution was implemented, the partion for the filesystem had to fit around this bootloader code.
This was done by partioning the drive with one partition starting after the 25KiB mark and subsiquently formatting that partition to FAT32.
The advantage of this is that (making use of kpartx) we can use regular formatting tools and we do not need to manually set FAT variables in boot.asm.
The tradeoff however is that since the partion data acording to the MBR scheme is also stored in the first partion, we now have to manually define the variables in boot.asm that will partion the drive.
I have found the best way to find what these values should be is to partition the compiled output disk image using GParted, then using hexdump to find what the values should be.   

While creating the bootloader, it was always meant to be the final OS/kernel, up until I understood we _need_ a higher-half kernel to be able to execute external programs.
This is why the bootloader has interupt support, an unfinished ELF.c file and supports full IO.
Moving to the kernel a lot of stuff has to be redone like interupts and memory manager initialization.
Since the executing code has to be run in mapped memory, we need to map the memory that is going to store the new pages before jumping to the new higher-half kernel. 
For this reason the third stage maps this (hardcoded) area.  
The kernel takes a parameter block, the idea is to have the bootloader and kernel agree on the parameter structure. Then pass relavent things like the VBE info, maybe E820 and where to store new memory pages (this is currently hardcoded)


While basically all code is written by myself, inspired or based on others work and help (love wiki.OSDEV.org). I _never_ understood IDE and the entire file is coppied form the wiki.
It is the only code that in no sense got written or edited by me.
``` dot
digraph O {

   rankdir=LR

   subgraph cluster_boot_asm {
      style=filled;
      color=lightgrey;
      "Booting autismoS...";
      "print #2";
      label = "boot.asm";
   }
   
   subgraph cluster_disk_asm {
      "read_disk";
      label = "disk.asm";
   }
   
   subgraph cluster_stage2_asm {
      label = "stage2.asm";
      subgraph cluster_16_bits {
      label = "16 bits";
         A20;
         E820;
         "Load basic GDT";
         subgraph cluster_vesa{
            "get VESA controller info";
            "get info about VESA mode";
            "see if VESA mode is correct" [shape=box];
            label="vesa stuff"
            "get VESA controller info" -> "get info about VESA mode";
            "get info about VESA mode" -> "see if VESA mode is correct";
            "see if VESA mode is correct" -> "get info about VESA mode" [label="no"]
         
         }
         "print #2" -> A20;
         A20 -> "Load basic GDT";
         "Load basic GDT" -> "get VESA controller info";
         "see if VESA mode is correct" -> E820 [label="yes"]
      }
      subgraph cluster_32_bits {
         label = "32 bits";
         setupPaging;
         editBasicGDT;
         checkCpuid;
         checkCpuid -> setupPaging -> editBasicGDT;
      }
      E820 -> checkCpuid;
      
      subgraph cluster_64_bits {
         label="64 bits"
         "setupStack (0x90000)"
      }
      
      subgraph cluster_handleIRQISR {
         label = "irq/isr";
         Bisr_common;
         Birq_common;
      
      }
      
   }
   
   subgraph cluster_stage3 {
     label = "stage3.c" 
      "_start()";
   }
   
   subgraph cluster_bootIO{
      label = "io.c"
      BioInit;
      Bcls;
      Bkprintf;
      Bprint;
      Bformat;
      Bi2h;
      Bi2a;
      Bprintch;
      Bkprintf -> Bformat;
      Bformat -> Bprintch [label="(ch != %)"]
      Bformat -> Bprintch [label="%c"]
      Bformat -> Bi2a -> Bprint
      Bformat -> Bi2h -> Bkprintf
   }
   
   subgraph cluster_bootIDE {
      label = "ide.c";
      BideInit;
      Bide_write;
      Bide_read;
      Bide_read_buffer;
      Bide_read_sectors;
      Bide_polling
      Bide_ata_access;
      Bide_print_error;
      BideInit -> Bide_write;
      BideInit -> Bide_read;
      BideInit -> Bide_read_buffer;
      Bide_read -> Bide_write;
      Bide_write -> Bide_write;
      Bide_read_buffer -> Bide_write;
      Bide_read_buffer -> Bide_read;
      Bide_read_sectors -> Bide_ata_access; 
      Bide_read_sectors -> Bide_print_error
      Bide_ata_access -> Bide_write
      Bide_ata_access -> Bide_read;
      Bide_ata_access -> Bide_polling;
   }
   
   subgraph cluster_bootutils{
      label = "utils.c"
      Binb;
      Binsl;
      Boutls;
      Boutb;
      Binvlpg;
   }
   
   subgraph cluster_bootDebug{
      label = "debug.c"
      BinitSerial;
   }
   
   subgraph cluster_bootMemory{
      label = "memory.c"
      BpagingInit;
      BmapPage;
      Bkmalloc;
      Balloc_page;
      Bmempcy;
      BpagingInit -> BmapPage;
      Bkmalloc -> BmapPage;
      BmapPage -> Balloc_page;
      BmapPage -> Binvlpg;
   }
   
   subgraph cluster_bootFat{
      label = "fat.c"
      BfatInit;
      BopenFile;
      BloadClusterChain;
      BopenFile -> BloadClusterChain
   }
   
   start -> "Booting autismoS...";
   "Booting autismoS..." -> "read_disk";
   read_disk -> "print #2";
   editBasicGDT -> "setupStack (0x90000)";
   "setupStack (0x90000)" -> "_start()";
   Bide_read -> Binb
   Bide_write -> Boutb
   Bide_read_buffer -> Binsl
   BfatInit -> Bkmalloc;
   BfatInit -> Bide_read_sectors;
   BfatInit -> BmapPage;
   BloadClusterChain -> Bide_read_sectors;
   "_start()" -> BioInit [label=1];
   "_start()" -> Bcls [label=2];
   "_start()" -> BideInit [label=3];
   "_start()" -> BinitSerial [label=4];
   "_start()" -> BpagingInit [label=5];
   "_start()" -> BfatInit [label=6];
   "_start()" -> BmapPage [color=blue];
   "_start()" -> BopenFile [label=7,color=blue];
   "_start()" -> Bkprintf [color=blue];
   "_start()" -> Bkmalloc [color=blue]
   
   
   subgraph cluster_kernel{
      label = "kernel"
      
      subgraph cluster_main{
         label = "main.c"
         main;
      }

      subgraph cluster_debug{
         label = "debug.c"
         writeSerial;
      }

      subgraph cluster_ELF{
         label = "ELF.c"
         loadElf;
         executeElf;
         loadElf -> executeElf;
      }
   
      subgraph cluster_fat{
         label = "fat.c"
         fatInit;
         loadFile;
         findFile;
         openFile;
         loadClusterChain;
         clusterToLba;
         loadFile -> findFile;
         loadFile -> loadClusterChain;
         openFile -> findFile;
         openFile -> loadClusterChain;
      }

      subgraph cluster_GDT{
         label = "GDT.c"
         loadGDT;
      }

      subgraph cluster_ide{
         label = "ide.c";
         ideInit;
         ide_write;
         ide_read;
         ide_read_buffer;
         ide_read_sectors;
         ide_polling
         ide_ata_access;
         ide_print_error;
         ideInit -> ide_write;
         ideInit -> ide_read;
         ideInit -> ide_read_buffer;
         ide_read -> ide_write;
         ide_write -> ide_write;
         ide_read_buffer -> ide_write;
         ide_read_buffer -> ide_read;
         ide_read_sectors -> ide_ata_access; 
         ide_read_sectors -> ide_print_error
         ide_ata_access -> ide_write
         ide_ata_access -> ide_read;
         ide_ata_access -> ide_polling;
      }

      subgraph cluster_interrupt{
         label = "interrupt.c"
         createIDT;
         idt_set_descriptor;
         registerInterupt;
         exceptionHandler;
         irq_handler;
         createIDT -> idt_set_descriptor;
      }

      subgraph cluster_interruptASM{
         label = "interrupt.asm"
         1 [color=green];
         2 [color=green];
         "..." [color=green];
         39 [color=green];
         isr_stub;
         isr_stub_err;
         irq_stub;
         isr_common;
         irq_common;
         1 -> isr_stub;
         2 -> isr_stub;
         "..." -> isr_stub;
         "..." -> isr_stub_err;
         "..." -> irq_stub;
         39 -> irq_stub;
         isr_stub -> isr_common;
         isr_stub_err -> isr_common;
         irq_stub -> irq_common;
      }

      subgraph cluster_io{
         label = "io.c"
         kprintf;
         ioInit;
         putch;
         print;
         printch;
         format;
         i2a;
         i2h;
         kprintf -> format;
         format -> printch [label="(ch != %)"];
         format -> print;
         format -> printch [label="%c"];
         format -> i2a -> print;
         format -> i2h -> print;
         print -> printch -> print
         i2h -> kprintf
         printch -> putch
      }
      
      subgraph cluster_memory{
         label = "memory.c"
         pagingInit;
         alloc_page;
         mmPage;
         mapPage;
         kmalloc;
         memcpy;
         pagingInit -> alloc_page;
         pagingInit -> mmPage;
         pagingInit -> mapPage;
         kmalloc -> mapPage;
         mapPage -> mmPage;
         mmPage -> alloc_page;
         mmap -> mapPage;
      }

      subgraph cluster_PCI{
         label = "PCI.c"
         main;
      }

      subgraph cluster_PIC{
         label = "PIC.c"
         PIC_sendEOI;
         PIC_remap
      }

      subgraph cluster_syscall{
         label = "syscall.c"
         main;
      }

      subgraph cluster_utils{
         label = "utils.c"
         outb;
         inb;
         io_wait;
         invlpg
      }
      
      printch -> writeSerial;
      putch -> writeSerial;
      writeSerial -> outb;
      PIC_sendEOI -> outb;
      PIC_remap -> io_wait;
      PIC_remap -> outb;
      createIDT -> PIC_remap;
      createIDT -> inb;
      irq_handler -> PIC_sendEOI;
      isr_common -> exceptionHandler;
      irq_common -> irq_handler;
      mmPage -> invlpg;
      loadGDT -> kmalloc;
      fatInit -> kmalloc;
      fatInit -> ide_read_sectors;
      fatInit -> mapPage;
      loadFile -> kmalloc;
      loadClusterChain -> ide_read_sectors;
      loadElf -> loadFile;
      executeElf -> mapPage;
      executeElf -> memcpy;
      ide_read -> inb;
      ide_write ->outb;
      main -> kprintf [color=blue];
      main -> PIC_sendEOI [label=1];
      main -> createIDT [label=2];
      main -> pagingInit [label=3];
      main -> mapPage [color=blue];
      main -> loadGDT [label=4];
      main -> ideInit [label=5];
      main -> fatInit [label=6];
      main -> loadElf [color=blue];
   
   }
   "_start()" -> main [label=8,color=red]
  
   start [shape=Mdiamond];
}
```
