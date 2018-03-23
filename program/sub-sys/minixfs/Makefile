CC = gcc
LD = ld
CFLAGS = -nostdinc -fno-builtin -m32 -Wall -g -O0
LDFLAGS = -m elf_i386
SRCS = *.c
OBJECTS = head.o main.o hard_disk.o partion.o superblk.o inode.o bitmap.o

hd.img : boot kernel
	objcopy -j .text -O binary boot boot.img
	objcopy -j .text -O binary kernel kernel.img
	dd if=boot.img of=hd.img bs=432 count=1 conv=notrunc
	dd if=kernel.img of=hd.img bs=512 count=60 seek=1 conv=notrunc

%.o : %.S
	$(CC) $(CFLAGS) -c $^

%.o : %.c
	$(CC) $(CFLAGS) -c $^

boot : boot.o
	$(LD) $(LDFLAGS) -Ttext 0x00 $^ -o boot

kernel : head.o $(OBJECTS)
	$(LD) $(LDFLAGS) -Ttext 0x00 $^ -o kernel

.PHONY:
depend:
	makedepend $(SRCS) -I ./ -Y

.PHONY:
clean:
	rm -f kernel.img kernel boot.img boot boot.o $(OBJECTS) *.gch
# DO NOT DELETE

bitmap.o: bitmap.h defs.h hard_disk.h
hard_disk.o: asm.h defs.h
inode.o: inode.h defs.h
main.o: defs.h hard_disk.h partion.h superblk.h bitmap.h
partion.o: asm.h defs.h partion.h
superblk.o: superblk.h defs.h hard_disk.h asm.h
