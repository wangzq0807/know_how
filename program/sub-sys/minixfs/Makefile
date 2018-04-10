CC = gcc
LD = ld
CFLAGS = -g -O0  -m32 -Wall -Werror -ffreestanding -nostdinc
LDFLAGS = -m elf_i386
SRCS = *.c
OBJECTS = head.o main.o hard_disk.o partion.o superblk.o inode.o bitmap.o arch.o log.o intr.o

# boot只需要代码区
# kernel除了代码区以外还需要数据区和bss区等
hd.img : boot kernel
	objcopy -j .text -O binary boot boot.img
	objcopy -O binary kernel kernel.img
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

arch.o: arch.h defs.h asm.h
bitmap.o: bitmap.h defs.h hard_disk.h
buffer.o: buffer.h defs.h hard_disk.h
hard_disk.o: asm.h defs.h
inode.o: inode.h defs.h hard_disk.h asm.h partion.h superblk.h log.h
log.o: log.h defs.h
main.o: defs.h hard_disk.h partion.h superblk.h bitmap.h inode.h arch.h log.h
partion.o: asm.h defs.h partion.h hard_disk.h
superblk.o: superblk.h defs.h hard_disk.h asm.h
