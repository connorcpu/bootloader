.PHONY: default compile iso run image clean

C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h *.c)
OBJ = $(C_SOURCES:%.c=bin/%.o)

default: run

image: compile
	dd if=bin/comb.o of=bin/qemu.img
	truncate -s 720K bin/qemu.img

run: clean image
	qemu-system-x86_64 bin/qemu.img > /dev/null 2>&1

compile: ${OBJ} 
	nasm boot.asm -f bin -o bin/boot.o
	nasm stage2.asm -f elf64 -o bin/stage2.o
	ld -Ttext 0x8000 bin/stage2.o $^ -o bin/kernel.o
	objcopy -O binary bin/kernel.o bin/kernel.bin
	cat bin/boot.o bin/kernel.bin > bin/comb.o

bin/%.o: %.c ${HEADERS}
	gcc -ffreestanding -mno-red-zone -m64 -c $< -o $@
#	clang -std=c17 -ffreestanding -mno-red-zone -mcmodel=kernel -fno-stack-protector -fpic -fpie -gdwarf -fwrapv -Werror -pipe -c $< -o $@
	
iso: bin/os.iso

bin/os.iso: compile
	dd if=bin/comb.o of=osfs/iso.o
	truncate -s 1200K osfs/iso.o
	mkisofs -jcharset ASCII -o bin/os.iso -b iso.o osfs/ 

clean:
	rm bin/* -f
