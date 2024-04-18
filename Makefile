.PHONY: default compile iso run image c_compile
C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h *.c)
OBJ = $(C_SOURCES:.c=.o)
default: run

image: compile
	dd if=bin/comb.o of=bin/qemu.img
	truncate -s 720K bin/qemu.img

run: image
	qemu-system-x86_64 bin/qemu.img > /dev/null 2>&1

compile: ${OBJ}
	nasm boot.asm -f bin -o bin/boot.o
	nasm stage2.asm -f elf64 -o bin/stage2.o
	#gcc -ffreestanding -mno-red-zone -m64 -c stage3.c io.h io.c -o bin/stage3.c.o
	ld -Ttext 0x8000 bin/stage2.o $^ -o bin/stage3.o
	objcopy -O binary bin/stage3.o bin/stage3.bin
	cat bin/boot.o bin/stage3.bin > bin/comb.o

%.o: %.c ${HEADERS}
	gcc -ffreestanding -mno-red-zone -m64 -c $< -o $@
	
iso: bin/os.iso

bin/os.iso: compile
	dd if=bin/comb.o of=osfs/iso.o
	truncate -s 1200K osfs/iso.o
	mkisofs -jcharset ASCII -o bin/os.iso -b iso.o osfs/ 

	
