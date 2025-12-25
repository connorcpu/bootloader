.PHONY: default compile iso run image clean

C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h *.c)
OBJ = $(C_SOURCES:%.c=bin/%.o)

compile: ${OBJ}
	ld -Ttext 0xC0000000 $^ -o bin/main.bin
	objcopy -O binary bin/main.bin bin/kernel.bin

bin/%.o: %.c ${HEADERS}
	x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -c $< -o $@
