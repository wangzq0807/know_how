cmake_minimum_required (VERSION 3.0)
enable_language (ASM)

# 编译选项
add_compile_options (-g -O2 -m32 -Wall -Werror -ffreestanding -nostdinc)
# 链接选项
set (LD_FLAGS "-m elf_i386 -Ttext 0x0")

# 使用ld作为链接器
set(CMAKE_C_LINK_EXECUTABLE
    "ld ${LD_FLAGS} <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# 源文件
include_directories (include)
set (VGA_SRCS
    boot.S
    main.c
    vga.c)
# 目标文件
add_executable (vga
    ${VGA_SRCS})
# 创建软盘
add_custom_target (floopy.img ALL)
add_custom_command (TARGET floopy.img PRE_BUILD
    COMMAND objcopy -O binary vga vga.img
    COMMAND dd if=/dev/zero of=floopy.img bs=512 count=2880
    COMMAND dd if=vga.img of=floopy.img bs=512 conv=notrunc)
add_dependencies (floopy.img vga)

