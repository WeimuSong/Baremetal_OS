# team 34
# Weimu Song
# Yuqi Zhou

all: kernel makekernel run

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o 

kernel: kernel.o kernel_asm.o
	ld86 -o kernel -d kernel.o kernel_asm.o 

makekernel: kernel bootload.asm message.txt
	nasm bootload.asm
	dd if=/dev/zero of=floppya.img bs=512 count=2880
	dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
	dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc

run: floppya.img
	qemu-system-i386 -fda floppya.img -boot order=a &

clean:
	rm -rf *.o *~

