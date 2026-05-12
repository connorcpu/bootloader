.PHONY: default run compile

default: run

C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h *.c)
OBJ := $(C_SOURCES:%.c=bin/%.o)

run: clean ${OBJ}
	cp bin/* ../bin/

bin/%.o: %.c ${HEADERS}
	x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -mcmodel=large -c $< -o $@

clean: 
	rm bin/* -f
