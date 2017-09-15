GENDEV?=/opt/gendev
GCC_VER?=6.3.0

MAKE?=make
LIB?=lib
GENGCC_BIN=$(GENDEV)/bin
GENBIN=$(GENDEV)/bin

CC = $(GENGCC_BIN)/m68k-elf-gcc
AS = $(GENGCC_BIN)/m68k-elf-as
AR = $(GENGCC_BIN)/m68k-elf-ar
LD = $(GENGCC_BIN)/m68k-elf-ld
RANLIB = $(GENGCC_BIN)/m68k-elf-ranlib
OBJC = $(GENGCC_BIN)/m68k-elf-objcopy
BINTOS = $(GENBIN)/bintos
RESCOMP= $(GENBIN)/rescomp
XGMTOOL= $(GENBIN)/xgmtool
PCMTORAW = $(GENBIN)/pcmtoraw
WAVTORAW = $(GENBIN)/wavtoraw
SIZEBND = $(GENBIN)/sizebnd
ASMZ80 = $(GENBIN)/zasm

RM = rm -f
NM = nm
NM2WCH = nm2wch
MKISOFS = mkisofs

SCD_LOADER = scd/LukeProjectCD

OPTION = --std=c11
INCS = -I. -I./include -I$(GENDEV)/m68k-elf/include -I$(GENDEV)/m68k-elf/m68k-elf/include -I$(GENDEV)/sgdk/inc -I$(GENDEV)/sgdk/res -Isrc -Ires
CCFLAGS = $(OPTION) -m68000 -Wall -c -fomit-frame-pointer -O0#2
HWCCFLAGS = $(OPTION) -m68000 -Wall -c -fomit-frame-pointer -O0#1
Z80FLAGS = -vb2
ASFLAGS = -m68000 --register-prefix-optional
#LIBS =  -L$(GENDEV)/m68k-elf/lib -L$(GENDEV)/m68k-elf/lib/gcc/m68k-elf/4.8.2 -L$(GENDEV)/m68k-elf/m68k-elf/lib -lmd -lc -lgcc -lnosys -lm
LIBS =  -L$(GENDEV)/m68k-elf/lib -L$(GENDEV)/m68k-elf/lib/gcc/m68k-elf/* -L$(GENDEV)/m68k-elf/m68k-elf/lib -L$(GENDEV)/sgdk1_3/lib -lmd -lnosys
LINKFLAGS = -T $(GENDEV)/ldscripts/md.ld -Map=output.map -nostdlib
SCDLINKFLAGS = -T scd/mdcd.ld -nostdlib
ARCHIVES = $(GENDEV)/sgdk1_3/lib/libmd.a


# Are we AMD64?
#LBITS := $(shell getconf LONG_BIT)
#ifeq ($(LBITS), 64)
#ARCHIVES += $(GENDEV)/m68k-elf/lib64/gcc/m68k-elf/$(GCC_VER)/libgcc.a
#else
#ARCHIVES += $(GENDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER)/libgcc.a
#endif

ARCHIVES += $(GENDEV)/$(LIB)/gcc/m68k-elf/$(GCC_VER)/libgcc.a

RESOURCES=
BOOT_RESOURCES=

BOOTSS=$(wildcard boot/*.s)
BOOTSS+=$(wildcard src/boot/*.s)
BOOT_RESOURCES+=$(BOOTSS:.s=.o)

SCDBOOTSS=$(wildcard scd/*.s)
SCDBOOTSS+=$(wildcard src/scd/*.s)
SCDBOOT_RESOURCES=$(SCDBOOTSS:.s=.o)

#BMPS=$(wildcard res/*.bmp)
#VGMS=$(wildcard res/*.vgm)
#RAWS=$(wildcard res/*.raw)
#PCMS=$(wildcard res/*.pcm)
#MVSS=$(wildcard res/*.mvs)
#TFDS=$(wildcard res/*.tfd)
#WAVS=$(wildcard res/*.wav)
RESS=$(wildcard res/*.res)
#WAVPCMS=$(wildcard res/*.wavpcm)
#BMPS+=$(wildcard *.bmp)
#VGMS+=$(wildcard *.vgm)
#RAWS+=$(wildcard *.raw)
#PCMS+=$(wildcard *.pcm)
#MVSS+=$(wildcard *.mvs)
#TFDS+=$(wildcard *.tfd)
#WAVS+=$(wildcard *.wav)
RESS+=$(wildcard *.res)
#WAVPCMS+=$(wildcard *.wavpcm)

#RESOURCES+=$(BMPS:.bmp=.o)
#RESOURCES+=$(VGMS:.vgm=.o)
#RESOURCES+=$(RAWS:.raw=.o)
#RESOURCES+=$(PCMS:.pcm=.o)
#RESOURCES+=$(MVSS:.mvs=.o)
#RESOURCES+=$(TFDS:.tfd=.o)
#RESOURCES+=$(WAVS:.wav=.o)
#RESOURCES+=$(WAVPCMS:.wavpcm=.o)
RESOURCES+=$(RESS:.res=.o)

CS=$(wildcard src/*.c)
SS=$(wildcard src/*.s)
S80S=$(wildcard src/*.s80)
CS+=$(wildcard *.c)
SS+=$(wildcard *.s)
S80S+=$(wildcard *.s80)
RESOURCES+=$(CS:.c=.o)
RESOURCES+=$(SS:.s=.o)
RESOURCES+=$(S80S:.s80=.o)

OBJS = $(RESOURCES)

all: out.bin

boot/sega.o: boot/rom_head.bin
	$(AS) $(ASFLAGS) boot/sega.s -o $@

scd/segacd.o:
	$(AS) $(ASFLAGS) scd/segacd.s -o $@


out.iso: out.elf_scd
	#
	# Create a sega cd image.  Limited to 256K or smaller Roms
	#
	$(NM) -n -S -t x out.elf_scd > out.nm
	$(OBJC) -O binary out.elf_scd out.bin
	$(SIZEBND) out.bin -sizealign 131072
	$(OBJC) -O binary out.elf_scd $(SCD_LOADER)/_filesystem/M_INIT.PRG
	$(SIZEBND) $(SCD_LOADER)/_filesystem/M_INIT.PRG -sizealign 131072
	$(MKISOFS) -iso-level 1 -o $(SCD_LOADER)/filesystem.img -pad $(SCD_LOADER)/_filesystem
	tail -c +32769 $(SCD_LOADER)/filesystem.img > $(SCD_LOADER)/filesystem.bin
	$(RM) -f $(SCD_LOADER)/filesystem.img
	cd $(SCD_LOADER) && $(AS) $(ASFLAGS) -M -ahlsm=listing.asm  main-us-as.asm -o out.iso
	tail -c +53 $(SCD_LOADER)/out.iso > out.iso
	$(RM) -f $(SCD_LOADER)/filesystem.bin

%.bin: %.elf
	$(OBJC) -O binary $< temp.bin
	dd if=temp.bin of=$@ bs=8K conv=sync

%.elf: $(OBJS) $(BOOT_RESOURCES)
	$(LD) -o $@ $(LINKFLAGS) $(BOOT_RESOURCES) $(ARCHIVES) $(OBJS) $(LIBS)

%.elf_scd: $(OBJS) $(SCDBOOT_RESOURCES)
	$(LD) -o $@ $(SCDLINKFLAGS) $(SCDBOOT_RESOURCES) $(ARCHIVES) $(OBJS) $(LIBS)

%.o80: %.s80
	$(ASMZ80) $(Z80FLAGS) -o $@ $<

%.c: %.o80
	$(BINTOS) $<

%.o: %.c
	$(CC) $(CCFLAGS) $(INCS) -c $< -o $@ #-Wa,-adhln=$@.s

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.s: %.bmp
	bintos -bmp $<

%.rawpcm: %.pcm
	$(PCMTORAW) $< $@

%.raw: %.wav
	$(WAVTORAW) $< $@ 16000

%.pcm: %.wavpcm
	$(WAVTORAW) $< $@ 22050

#%.tfc: %.tfd
#	$(TFMCOM) $<

#%.o80: %.s80
#	$(ASMZ80) $(FLAGSZ80) $< $@ out.lst

%.s: %.tfd
	$(BINTOS) -align 32768 $<

%.s: %.mvs
	$(BINTOS) -align 256 $<

%.s: %.esf
	$(BINTOS) -align 32768 $<

%.s: %.eif
	$(BINTOS) -align 256 $<

%.s: %.vgm
	$(BINTOS) -align 256 $<

%.s: %.raw
	$(BINTOS) -align 256 -sizealign 256 $<

%.s: %.rawpcm
	$(BINTOS) -align 128 -sizealign 128 -nullfill 136 $<

%.s: %.rawpcm
	$(BINTOS) -align 128 -sizealign 128 -nullfill 136 $<

%.s: %.res
	$(RESCOMP) $< $@

boot/rom_head.bin: boot/rom_head.o
	$(LD) $(LINKFLAGS) --oformat binary -o $@ $<


clean:
	$(RM) $(RESOURCES)
	$(RM) *.o *.bin *.elf *.elf_scd *.map *.iso
	$(RM) boot/*.o boot/*.bin
