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

