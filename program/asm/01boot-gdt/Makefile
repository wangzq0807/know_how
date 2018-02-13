boot.img : boot.elf
	# -j .text : 指定拷贝代码段
	# -O binary: 指定输出格式为二进制
	# 如果在ld中指定"-s --oformat binary"效果与objcopy一样
	objcopy -j .text -O binary boot.elf boot.img
boot.elf : boot.o
	# -m elf_i386 : 指定elf格式
	# -Ttext : 代码段链接地址
	ld -m elf_i386 -Ttext 0x00 boot.o -o boot.elf
boot.o : boot.S
	# --32 : 32位程序
	as -o boot.o boot.S --32

.PHONY:
clean:
	rm boot.img boot.elf boot.o
