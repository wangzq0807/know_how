CC = gcc
LD = ld
CFLAGS = -g -O2  -m32 -Wall -Werror -ffreestanding -nostdinc
LDFLAGS = -m elf_i386
INCLUDES = -I ./ -I ./include
BUILD_PATH = _build

# 汇编源文件
ASMSRCS = boot.S \
		  intr.S
# 查找所有的.c文件
SOURCES = $(shell find ./ -name '*.c' -printf '%T@\t%p\n' \
			| sort -k 1nr | cut -f2-)
# 自动生成的obj文件名, head.o需放在$(OBJECTS)第一位
OBJECTS = $(patsubst %.S, $(BUILD_PATH)/%.o, $(ASMSRCS))
OBJECTS += $(patsubst %.c, $(BUILD_PATH)/%.o, $(SOURCES))
# 自动生成depends文件名
DEPS = $(OBJECTS:.o=.d)

all : dirs
	@$(MAKE) floopy.img --no-print-directory

# boot只需要代码区
# kernel除了代码区以外还需要数据区和bss区等
floopy.img : kernel
	@objcopy -O binary kernel kernel.img
	@objdump -S kernel > kernel.sym
	@echo "将内核写入虚拟磁盘floopy.img..."
	@dd if=/dev/zero of=floopy.img bs=512 count=2880
	@dd if=kernel.img of=floopy.img bs=512 conv=notrunc
	@dd if=one_task of=floopy.img bs=512 conv=notrunc seek=40

# 内核的链接规则
kernel : $(OBJECTS)
	$(LD) $(LDFLAGS) -Ttext 0x7c00 $^ -o kernel

# 源文件编译规则
$(BUILD_PATH)/%.o : %.c
	$(CC) $(CFLAGS) -MP -MMD -c $< -o $@ $(INCLUDES)

$(BUILD_PATH)/%.o : %.S
	$(CC) $(CFLAGS) -MP -MMD -c $< -o $@ $(INCLUDES)

# 包含所有依赖
-include $(DEPS)

.PHONY: dirs
dirs:
	@echo "创建编译目录..."
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BUILD_PATH)

.PHONY:
clean:
	@$(RM) -rf kernel kernel.img kernel.sym floopy.img $(BUILD_PATH)/*
