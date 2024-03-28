default: 
	nasm boot.asm -f bin -o boot.o
	nasm stage2.asm -f bin -o stage2.o
	cat boot.o stage2.o > comb.o
	dd if=comb.o of=image.img
	truncate -s 720K image.img
	qemu-system-x86_64 -fda image.img -nographic
