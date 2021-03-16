kernel_source_files := $(shell find src/impl/kernel/ -name *.c)
kernel_object_files := $(patsubst src/impl/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

x86_64_c_source_files := $(shell find src/impl/x86_64/ -name *.c)
x86_64_c_object_files := $(patsubst src/impl/x86_64/%.c, build/x86_64/%.o, $(x86_64_c_source_files))

x86_64_asm_source_files := $(shell find src/impl/x86_64/ -name *.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o, $(x86_64_asm_source_files))

x86_64_object_files := $(x86_64_c_object_files) $(x86_64_asm_object_files)

include_dir := src/intf

MKDIRP = mkdir -p
CC := gcc
CFLAGS := -c -I $(include_dir) -ffreestanding -m64 -Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector -O2 -mno-red-zone
AS := nasm
ASFLAGS := -f elf64 -i $(include_dir)
LD := ld
LDFLAGS := -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld

$(kernel_object_files): build/kernel/%.o : src/impl/kernel/%.c $(wildcard src/intf/*.h)
	$(MKDIRP) $(dir $@) && \
	$(CC) $(CFLAGS) $< -o $@

$(x86_64_c_object_files): build/x86_64/%.o : src/impl/x86_64/%.c $(wildcard src/intf/*.h)
	$(MKDIRP) $(dir $@) && \
	$(CC) $(CFLAGS) $< -o $@

$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	$(MKDIRP) $(dir $@) && \
	$(AS) $(ASFLAGS) $< -o $@

.PHONY: build-x86_64
build-x86_64: $(kernel_object_files) $(x86_64_object_files)
	$(MKDIRP) dist/x86_64 && \
	$(LD) $(LDFLAGS) $(kernel_object_files) $(x86_64_object_files)
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso

.DEFAULT_GOAL := build-x86_64
