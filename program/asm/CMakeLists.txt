cmake_minimum_required (VERSION 3.0)
enable_language (ASM-ATT)

# 编译选项
add_compile_options ("--32")
# 链接选项
set (CMAKE_ASM-ATT_LINK_FLAGS
	"-m elf_i386 -Ttext 0x0")

add_subdirectory (01boot-gdt)

# 编译命令
#set (CMAKE_ASM-ATT_COMPILE_OBJECT
#	"<CMAKE_ASM-ATT_COMPILER> <INCLUDES> ${ASM_SYS_FLAGS} -o <OBJECT> <SOURCE>")
