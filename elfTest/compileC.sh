x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -c flappyBird.c -o flappyBird.o
#ld -Ttext 0x10400 test.o bin/iolib.a -o testlib.o
#ld -Ttext 0x10400 flappyBird.o -o flappyBird.bin
ld -T linker.ld flappyBird.o -o flappyBird.bin
#objcopy -O binary flappyBird.o ../osfs/game.exe
cp flappyBird.bin ../osfs/game.exe
