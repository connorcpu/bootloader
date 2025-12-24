x86_64-elf-gcc elfTest/flappyBird.c -nostdlib -ffreestanding -W -Lstdlib -lautism -o bin/testC.bin
cp bin/testC.bin osfs/test3.exe
