.PHONY: default compile iso run image clean

default: run

C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h *.c)
OBJ := $(C_SOURCES:%.c=bin/%.o)
OBJ := $(filter-out bin/main.o, $(OBJ))

run: clean compile
	cp bin/kernel.bin ../osfs/kernel.bin

compile: ${OBJ}
	x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -c main.c -o bin/main.o
	ld -T linker.ld $^
	cp bin/main.bin bin/kernel.bin
	#objcopy -O binary bin/main.bin bin/kernel.bin

bin/%.o: %.c ${HEADERS}
	x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -c $< -o $@

clean:
	rm bin/* -f
