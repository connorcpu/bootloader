default: 
	nasm boot.asm -f bin -o boot.o
	dd if=boot.o of=image.img
	qemu-system-x86_64 image.img -nographic
