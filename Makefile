DIR_DIST := dist
DIR_SRC := src
DIR_BUILD := build
DIR_INCLUDE := include
DIR_TARGET := target
DIR_ROOTFS := $(DIR_TARGET)/rootfs

MKDIRP = mkdir -p
CC := g++
CFLAGS := -c -iquote $(DIR_INCLUDE) -ffreestanding -m64 -Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector -O2 -pipe -mno-red-zone
AS := nasm
ASFLAGS := -f elf64
LD := ld.lld
LDFLAGS := -n -T $(DIR_TARGET)/linker.ld

gcc_source_files := $(shell find $(DIR_SRC) -name *.c -or -name *.cpp)
gcc_object_files := $(patsubst $(DIR_SRC)/%, $(DIR_BUILD)/%.o, $(gcc_source_files))

asm_source_files := $(shell find src -name *.asm)
asm_object_files := $(patsubst $(DIR_SRC)/%, $(DIR_BUILD)/%.o, $(asm_source_files))

object_files := $(gcc_object_files) $(asm_object_files)
include_files := $(shell find $(DIR_INCLUDE) -name *.h -or -name *.hpp)

$(DIR_BUILD)/ $(DIR_DIST)/: %:
	$(MKDIRP) $@

$(gcc_object_files): $(DIR_BUILD)/%.o : $(DIR_SRC)/% $(include_files) | $(DIR_BUILD)/
	$(MKDIRP) $(shell dirname $@)
	$(CC) $(CFLAGS) $< -o $@

$(asm_object_files): $(DIR_BUILD)/%.o : $(DIR_SRC)/% | $(DIR_BUILD)/
	$(MKDIRP) $(shell dirname $@)
	$(AS) $(ASFLAGS) $< -o $@

$(DIR_DIST)/kernel.bin: $(object_files) $(DIR_TARGET)/linker.ld | $(DIR_DIST)/
	$(LD) $(LDFLAGS) -o $@ $(object_files)

.PHONY: build
build: $(DIR_DIST)/kernel.bin
	cp $(DIR_DIST)/kernel.bin $(DIR_ROOTFS)/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o $(DIR_DIST)/kernel.iso $(DIR_ROOTFS)

.DEFAULT_GOAL := build
