x86_64-elf-gcc -nostdlib -ffreestanding -Wl,--oformat=binary elfTest/test.c -o bin/testC.bin
cp bin/testC.bin osfs/test3.exe
