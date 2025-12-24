.PHONY: default compile

default: compile

C_SOURCES = $(wildcard *.c)
OBJ = $(C_SOURCES:%.c=bin/%.o)
HEADERS = $(wildcard *.h *.c)

compile: ${OBJ}
	ar rcs libautism.a $^

bin/%.o: %.c ${HEADERS}
	x86_64-elf-gcc -ffreestanding -m64 -c $< -o $@
