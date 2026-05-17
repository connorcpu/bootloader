.PHONY: default

default: run

C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h *.c)
OBJ := $(C_SOURCES:%.c=bin/%.o)

run: clean compile

compile: ${OBJ}
	ar rcs bin/libautism.a $^
	

bin/%.o: %.c ${HEADERS}
	x86_64-elf-gcc -ffreestanding -m64 -mcmodel=large -c $< -o $@

clean: 
	rm bin/* -f
