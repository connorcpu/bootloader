nasm elfTest/test.asm -f elf64 -o elfTest/test.o
objcopy -O binary elfTest/test.o osfs/test3.txt
