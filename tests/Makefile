.PHONY: basic

basic:
	mips-linux-gnu-gcc basic.c -ffreestanding -nostdlib -EL -O0 -g0 \
	-march=mips32 -mtune=mips32 \
	-mno-interlink-compressed -mno-mips16 -msoft-float -mno-mcu \
	-mno-synci -mno-ginv -mno-crc -mno-xpa -mno-virt -mno-eva -mno-mt \
	-mno-micromips -mno-mips3d -mno-mdmx -mno-smartmips -mno-dspr2 \
	-mno-dsp -mno-llsc -mno-shared -mno-plt -mno-xgot -mgp32 \
	-mno-odd-spreg -mno-llsc -mno-loongson-mmi -mno-loongson-ext \
	-mno-loongson-ext2 -mno-gpopt -mno-mad \
	-mno-branch-likely -mno-fp-exceptions -mno-synci -mrelax-pic-calls \
	-mno-frame-header-opt -mno-lxc1-sxc1 -mno-madd4 \
\
	-mfused-madd \
\
	-S -o basic.s
	mips-linux-gnu-as -mips32 -O0 -EL basic.s -o basic.o
	mips-linux-gnu-strip -s basic.o
	mips-linux-gnu-objcopy -O binary -j .text basic.o basic.dat
	rm -f basic.o
	du -sb basic.dat
	od -tx4 -An -w4 -v basic.dat

input:
	mips-linux-gnu-as -mips32 -OO -EL input.s -o input.o
	mips-linux-gnu-objcopy -O binary -j .text input.o input.dat
	rm -f input.o

	mips-linux-gnu-as -mips32 -OO -EL int1.s -o int1.o
	mips-linux-gnu-objcopy -O binary -j .text int1.o int1.dat
	rm -f int1.o

	od -tx4 -Ax -w4 -v input.dat
	od -tx4 -Ax -w4 -v int1.dat
