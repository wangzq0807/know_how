CC = gcc
LD = ld
CFLAGS = -g -O0  -m32 -Wall -Werror -ffreestanding -nostdinc
LDFLAGS = -m elf_i386
BUILD_PATH = _build
# 查找所有的.c文件
SOURCES = $(shell find ./ -name '*.c' -printf '%T@\t%p\n' \
			| sort -k 1nr | cut -f2-)
# 自动生成obj文件名(head.o需放在第一位)
OBJECTS = $(BUILD_PATH)/head.o \
		  $(BUILD_PATH)/intr.o
OBJECTS += $(SOURCES:./%.c=$(BUILD_PATH)/%.o)
# 自动生成depends文件名
DEPS = $(OBJECTS:.o=.d)

# boot只需要代码区
# kernel除了代码区以外还需要数据区和bss区等
hd.img : boot kernel
	@objcopy -j .text -O binary boot boot.img
	@objcopy -O binary kernel kernel.img
	@echo "将内核写入虚拟磁盘hd.img..."
	@dd if=boot.img of=hd.img bs=432 count=1 conv=notrunc
	@dd if=kernel.img of=hd.img bs=512 count=60 seek=1 conv=notrunc
	@$(RM) boot.img kernel.img

# 编译规则
$(BUILD_PATH)/%.o : %.c
	$(CC) $(CFLAGS) -MP -MMD -c $< -o $@

$(BUILD_PATH)/%.o : %.S
	$(CC) $(CFLAGS) -c $^ -o $@

# 引导程序链接规则
boot : $(BUILD_PATH)/boot.o
	$(LD) $(LDFLAGS) -Ttext 0x00 $^ -o boot

# 内核的链接规则
kernel : $(OBJECTS)
	$(LD) $(LDFLAGS) -Ttext 0x00 $^ -o kernel

# 包含所有依赖
-include $(DEPS)

.PHONY:
clean:
	@$(RM) kernel boot $(BUILD_PATH)/*
