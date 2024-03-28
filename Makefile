# Directory Variables
BIN_DIR := bin
BUILD_DIR := build

# Source and Output Files
BOOT_SRC := ./src/bootloader/boot.asm
BOOT_BIN := $(BIN_DIR)/boot.bin
KERNEL_SRC := ./src/kernel.asm
KERNEL_OBJ := $(BUILD_DIR)/kernel.asm.o
KERNEL_BIN := $(BIN_DIR)/kernel.bin
OS_BIN := $(BIN_DIR)/os.bin

LINKER := ./src/linker.ld

FILES = $(KERNEL_OBJ)

# Tool Variables
QEMU := qemu-system-x86_64

.PHONY: all test clean setup

all: setup $(BOOT_BIN) $(KERNEL_BIN)
	rm -rf $(OS_BIN)
	dd if=$(BOOT_BIN) >> $(OS_BIN)
	dd if=$(KERNEL_BIN) >> $(OS_BIN)
	dd if=/dev/zero bs=512 count=100 >> $(OS_BIN) # Fills the rest with 0 to for a sector

setup:
	mkdir -p $(BIN_DIR) $(BUILD_DIR)

$(KERNEL_BIN): $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o $(BUILD_DIR)/kernelfull.o
	i686-elf-gcc -T $(LINKER) -o $(KERNEL_BIN) -ffreestanding -O0 -nostdlib $(BUILD_DIR)/kernelfull.o

$(BOOT_BIN): $(BOOT_SRC)
	nasm -f bin $< -o $@

$(KERNEL_OBJ): $(KERNEL_SRC)
	nasm -f elf -g $< -o $@

test: all
	$(QEMU) -hda $(OS_BIN)

clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*
