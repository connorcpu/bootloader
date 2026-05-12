.PHONY: default compile iso run image clean

default: run

C_SOURCES = $(wildcard *.c)
C_SOURCES += $(wildcard syscalls/*.c)
HEADERS = $(wildcard *.h *.c)
HEADERS += $(wildcard syscalls/*.h syscalls/*.c)
OBJ := $(C_SOURCES:%.c=bin/%.o)
SYSOBJ := $(wildcard syscalls/bin/*.o)
COMPOBJ := $(wildcard bin/*.o)
COMPOBJ := $(filter-out bin/main.o, $(COMPOBJ))
OBJ := $(filter-out bin/main.o, $(OBJ))

run: clean compile test
	cp bin/kernel.bin ../osfs/kernel.bin

compile: ${OBJ}
	x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -mcmodel=large -c main.c -o bin/main.o
	#$(MAKE) -C syscalls
	ld -T linker.ld $^
	#ld -T linker.ld $(COMPOBJ)
#	cp bin/main.bin bin/kernel.bin
	objcopy -O binary bin/main.bin bin/kernel.bin

bin/%.o: %.c ${HEADERS}
	x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -mcmodel=large -c $< -o $@

test:
	nasm ../elfTest/syscalltest.asm -f elf64 -o bin/syscall.elf
	ld -o ../osfs/syscall.elf bin/syscall.elf

clean:
	rm ../osfs/* -f
	rm bin/* -rf
	mkdir bin/syscalls
