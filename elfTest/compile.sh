nasm elfTest/test.asm -f elf64 -o elfTest/test.o
x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -c elfTest/test.c -o elfTest/testC.o
ld elfTest/test.o elfTest/testC.o -o elfTest/comb.o 
objcopy -O binary elfTest/comb.o osfs/test3.exe
