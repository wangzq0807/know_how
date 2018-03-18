CC = gcc
LD = ld
CFLAGS = -m32 -Wall -g -O2
LDFLAGS = -m elf_i386
SRCS = hard_disk.c main.c
OBJECTS = head.o main.o hard_disk.o

hd.img : boot kernel
	objcopy -j .text -O binary boot boot.img
	objcopy -j .text -O binary kernel kernel.img
	dd if=boot.img of=hd.img bs=432 count=1 conv=notrunc
	dd if=kernel.img of=hd.img bs=512 count=16 seek=1 conv=notrunc

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
	rm -f kernel.img kernel boot.img  boot boot.o $(OBJECTS)
# DO NOT DELETE

hard_disk.o: asm.h defs.h
