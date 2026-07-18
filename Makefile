.PHONY: default compile iso run image clean

C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h *.c)
OBJ = $(C_SOURCES:%.c=bin/%.o)

default: run

image: compile
	dd if=bin/comb.o of=bin/qemu.img
	truncate -s 100M bin/qemu.img
	sudo kpartx -a bin/qemu.img 
	mkfs.fat -F32 -v -I '/dev/mapper/loop0p4' -n 'AUTISM FS  '
	sudo mount /dev/mapper/loop0p4 /media/osfat 
	sudo cp osfs/* /media/osfat 
	sudo umount /media/osfat
	sudo kpartx -d bin/qemu.img

run: clean image
#	qemu-system-x86_64 -hda bin/qemu.img  > /dev/null 2>&1
	#-hdd does funcy shit
	qemu-system-x86_64 -drive file=bin/qemu.img,if=ide,index=0,media=disk,format=raw -serial stdio -m 128M |& tee serial.log

compile: ${OBJ} 
	nasm boot.asm -f bin -o bin/boot.o -w+zeroing
	nasm stage2.asm -f elf64 -o bin/stage2.o -w+zeroing
	ld -Ttext 0x8000 bin/stage2.o $^ -o bin/kernel.o
	objcopy -O binary bin/kernel.o bin/kernel.bin
	cat bin/boot.o bin/kernel.bin > bin/comb.o
#	elfTest/compileC.sh
	$(MAKE) -C kernel 

bin/%.o: %.c ${HEADERS}
#	gcc -ffreestanding -mno-red-zone -m64 -c $< -o $@
	x86_64-elf-gcc -ffreestanding -Wint-to-pointer-cast -mno-red-zone -m64 -c $< -o $@
#	clang -std=c17 -ffreestanding -mno-red-zone -mcmodel=kernel -fno-stack-protector -fpic -fpie -gdwarf -fwrapv -Werror -pipe -c $< -o $@
	
#iso: bin/os.iso
iso: clean image
	dd if=/dev/zero of=bin/diskISO.iso bs=512 count=200000
	dd if=bin/qemu.img of=bin/diskISO.iso conv=notrunc bs=512 seek=0 count=1

#VDI does get executed, just doesn't quite work
vbox: clean image
	qemu-img convert -f raw -O vdi bin/qemu.img bin/vbox.vdi
	vboxmanage internalcommands sethduuid bin/vbox.vdi 9188b32b-d649-4a2a-baaa-b696e296be9a
	vboxmanage startvm "operating system test"

bin/os.iso: compile
	dd if=bin/comb.o of=osfs/iso.o
	truncate -s 1200K osfs/iso.o
	mkisofs -no-emul-boot -jcharset ASCII -o bin/os.iso -b iso.o osfs/ 

clean:
	rm bin/* -f
	rm serial.log -f
	rm bochsSerial.txt
	touch bochsSerial.txt
