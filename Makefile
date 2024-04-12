.PHONY: default compile iso run image
default: run

image: compile
	dd if=bin/comb.o of=bin/qemu.img
	truncate -s 720K bin/qemu.img

run: image
	qemu-system-x86_64 bin/qemu.img > /dev/null 2>&1

compile:
	nasm boot.asm -f bin -o bin/boot.o
	nasm stage2.asm -f bin -o bin/stage2.o
	cat bin/boot.o bin/stage2.o > bin/comb.o
	
iso: bin/os.iso

bin/os.iso: compile
	dd if=bin/comb.o of=osfs/iso.o
	truncate -s 1200K osfs/iso.o
	mkisofs -jcharset ASCII -o bin/os.iso -b iso.o osfs/ 

	
