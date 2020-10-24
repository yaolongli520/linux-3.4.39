cmd_lib/ldiv.o := arm-linux-gcc -Wp,-MD,lib/.ldiv.o.d  -nostdinc -isystem /opt/FriendlyARM/toolchain/4.9.3/lib/gcc/arm-cortexa9-linux-gnueabihf/4.9.3/include -Iinclude  -I/root/uboot_nanopi2/arch/arm/include -D__KERNEL__ -DCONFIG_SYS_TEXT_BASE=0x42C00000 -Wall -Wstrict-prototypes -Wno-format-security -fno-builtin -ffreestanding -Os -fno-stack-protector -g -fstack-usage -Wno-format-nonliteral -Werror=date-time -DCONFIG_ARM -D__ARM__ -mcpu=cortex-a9 -march=armv7-a -ffunction-sections -fdata-sections -fno-common -ffixed-r9 -Wno-unused-but-set-variable -mno-unaligned-access -fno-short-enums -fstrict-aliasing -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/s5p4418/prototype/module -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/s5p4418/prototype/base -I/root/uboot_nanopi2/arch/arm/include/asm/arch-s5p4418 -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/common -I/root/uboot_nanopi2/arch/arm/cpu/slsiap/devices -I/root/uboot_nanopi2/board/s5p4418/common -D__LINUX__ -DNX_RELEASE -D__SUPPORT_MIO_UBOOT__ -D__SUPPORT_MIO_UBOOT_CHIP_S5P4418__ -I/root/uboot_nanopi2/board/s5p4418/nanopi2/include -pipe    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(ldiv)"  -D"KBUILD_MODNAME=KBUILD_STR(ldiv)" -c -o lib/ldiv.o lib/ldiv.c

source_lib/ldiv.o := lib/ldiv.c

deps_lib/ldiv.o := \

lib/ldiv.o: $(deps_lib/ldiv.o)

$(deps_lib/ldiv.o):
