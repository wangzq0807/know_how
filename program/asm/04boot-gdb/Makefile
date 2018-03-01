floopy.img : boot.elf head.elf
	# -j .text : 指定拷贝代码段
	# -O binary: 指定输出格式为二进制
	# 如果在ld中指定"-s --oformat binary"效果与objcopy一样
	objcopy -j .text -O binary boot.elf boot.img
	objcopy -j .text -O binary head.elf head.img
	# 创建17个扇区的软盘
	dd if=/dev/zero of=floopy.img bs=512 count=17
	# 1个启动扇区
	dd if=boot.img of=floopy.img bs=512 count=1 conv=notrunc
	# 16个系统扇区
	dd if=head.img of=floopy.img bs=512 count=16 seek=1 conv=notrunc

boot.elf : boot.o
	# -m elf_i386 : 指定elf格式
	# -Ttext : 代码段链接地址
	ld -m elf_i386 -Ttext 0x00 boot.o -o boot.elf
boot.o : boot.S
	# --32 : 32位程序
	as -o boot.o boot.S --32

head.elf : head.o main.o
	# -m elf_i386 : 指定elf格式
	# -Ttext : 代码段链接地址
	ld -m elf_i386 -Ttext 0x00 head.o main.o -o head.elf
head.o : head.S
	# --32 : 32位程序
	gcc -c -m32 -o head.o head.S
main.o : main.c
	gcc -g -O0 -c -m32 -o main.o main.c

.PHONY:
clean:
	rm boot.img boot.elf boot.o head.img head.elf head.o floopy.img main.o
