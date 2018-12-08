
set (BOOTGDT_SRCS
       	boot.s	)

# 生成目标boot
add_executable (boot
	${BOOTGDT_SRCS})

# 生成目标boot.img
add_custom_target (boot.img ALL)
add_custom_command (TARGET boot.img PRE_BUILD
	COMMAND objcopy -j .text -O binary boot boot.img)
add_dependencies (boot.img boot)

# 编译和链接参数
#target_compile_options (boot
#	PRIVATE
#	--32)

#target_link_libraries (boot
#	PRIVATE
#	"-m elf_i386"
#	"-Ttext 0x0")

