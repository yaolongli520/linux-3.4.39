cmd_arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.o := arm-linux-gcc -Wp,-MD,arch/arm/cpu/slsiap/s5p4418/prototype/module/.nx_alive.o.d  -nostdinc -isystem /opt/FriendlyARM/toolchain/4.9.3/lib/gcc/arm-cortexa9-linux-gnueabihf/4.9.3/include -Iinclude  -I/root/uboot_nanopi2/arch/arm/include -D__KERNEL__ -DCONFIG_SYS_TEXT_BASE=0x42C00000 -Wall -Wstrict-prototypes -Wno-format-security -fno-builtin -ffreestanding -Os -fno-stack-protector -g -fstack-usage -Wno-format-nonliteral -Werror=date-time -DCONFIG_ARM -D__ARM__ -marm -mno-thumb-interwork -mabi=aapcs-linux -mword-relocations -mcpu=cortex-a9 -march=armv7-a -ffunction-sections -fdata-sections -fno-common -ffixed-r9 -Wno-unused-but-set-variable -mno-unaligned-access -fno-short-enums -fstrict-aliasing -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/s5p4418/prototype/module -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/s5p4418/prototype/base -I/root/uboot_nanopi2/arch/arm/include/asm/arch-s5p4418 -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/common -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/devices -I/root/uboot_nanopi2/board/s5p4418/common -D__LINUX__ -DNX_RELEASE -D__SUPPORT_MIO_UBOOT__ -D__SUPPORT_MIO_UBOOT_CHIP_S5P4418__ -I/root/uboot_nanopi2/board/s5p4418/nanopi2/include -pipe    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(nx_alive)"  -D"KBUILD_MODNAME=KBUILD_STR(nx_alive)" -c -o arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.o arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.c

source_arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.o := arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.c

deps_arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.o := \
  /root/uboot_nanopi2/arch/arm/cpu/slsiap/s5p4418/prototype/base/nx_chip.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_clkpwr.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/../base/nx_prototype.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/../base/nx_type.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/../base/nx_debug.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/../base/nx_chip.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/../base/nx_clockcontrol.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/../base/nx_bit_accessor.h \
  /root/uboot_nanopi2/arch/arm/cpu/slsiap/s5p4418/prototype/base/nx_type.h \
  arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.h \

arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.o: $(deps_arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.o)

$(deps_arch/arm/cpu/slsiap/s5p4418/prototype/module/nx_alive.o):
