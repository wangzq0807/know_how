CC = gcc
LD = ld
CFLAGS = -m32 -Wall -O0
LDFLAGS = -m elf_i386
SRCS = main.c
OBJECTS = boot.o head.o main.o

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
	$(LD) $(LDFLAGS) -Ttext 0x00 boot.o -o boot

kernel : head.o main.o
	$(LD) $(LDFLAGS) -Ttext 0x00 head.o main.o -o kernel

.PHONY:
depend:
	makedepend $(SRCS) -I ./ -Y

.PHONY:
clean:
	rm -f boot.img kernel.img boot kernel $(OBJECTS)
# DO NOT DELETE

main.o: defs.h
