CC = gcc
LD = ld
CFLAGS = -g -O2  -m32 -Wall -Werror -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS = -m elf_i386 -static
INCLUDES = -I ./ -I ./include
BUILD_PATH = _build

OBJECTS = one_task.o crt.o
# 自动生成depends文件名
DEPS = $(OBJECTS:.o=.d)

all: one_task
	@objdump -S one_task > one_task.sym

one_task: $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

# 源文件编译规则
%.o : %.c
	$(CC) $(CFLAGS) -MP -MMD -c $< -o $@ $(INCLUDES)

# 包含所有依赖
-include $(DEPS)

clean:
	-rm $(OBJECTS) $(DEPS) one_task
