# Directory Variables
BIN_DIR := bin
BUILD_DIR := build
BUILD_DIR_MEM := $(BUILD_DIR)/memory
BUILD_DIR_IDT := $(BUILD_DIR)/idt
INCLUDES = -I./src

# Source and Output Files
BOOT_SRC := ./src/bootloader/boot.asm
BOOT_BIN := $(BIN_DIR)/boot.bin
KERNEL_SRC := ./src/kernel.asm
KERNEL_OBJ := $(BUILD_DIR)/kernel.asm.o
KERNEL_BIN := $(BIN_DIR)/kernel.bin
OS_BIN := $(BIN_DIR)/os.bin
KERNEL_C_SRC := ./src/kernel.c
KERNEL_C_OBJ := $(BUILD_DIR)/kernel.o
IDT_ASM_OBJ := $(BUILD_DIR_IDT)/idt.asm.o
IDT_ASM_SRC := ./src/idt/idt.asm
IDT_OBJ := $(BUILD_DIR_IDT)/idt.o
IDT_SRC := ./src/idt/idt.c
MEM_OBJ := $(BUILD_DIR_MEM)/memory.o
MEM_SRC := ./src/memory/memory.c

LINKER := ./src/linker.ld
FILES = $(KERNEL_OBJ) $(KERNEL_C_OBJ) $(IDT_ASM_OBJ) $(IDT_OBJ) $(MEM_OBJ)

FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

# Tool Variables
QEMU := qemu-system-x86_64

.PHONY: all test clean setup

all: setup $(BOOT_BIN) $(KERNEL_BIN)
	rm -rf $(OS_BIN)
	dd if=$(BOOT_BIN) >> $(OS_BIN)
	dd if=$(KERNEL_BIN) >> $(OS_BIN)
	dd if=/dev/zero bs=512 count=100 >> $(OS_BIN) # Fills the rest with 0 to for a sector

setup:
	mkdir -p $(BIN_DIR) $(BUILD_DIR) $(BUILD_DIR_MEM) $(BUILD_DIR_IDT)

$(KERNEL_BIN): $(FILES)
	i686-elf-ld -g -relocatable $^ -o $(BUILD_DIR)/kernelfull.o
	i686-elf-gcc -T $(LINKER) -o $@ -ffreestanding -O0 -nostdlib $(BUILD_DIR)/kernelfull.o

$(BOOT_BIN): $(BOOT_SRC)
	nasm -f bin $< -o $@

$(KERNEL_OBJ): $(KERNEL_SRC)
	nasm -f elf -g $< -o $@

$(KERNEL_C_OBJ): $(KERNEL_C_SRC)
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

$(IDT_ASM_OBJ): $(IDT_ASM_SRC)
	nasm -f elf -g $< -o $@

$(IDT_OBJ): $(IDT_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c $< -o $@

$(MEM_OBJ): $(MEM_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

test: all
	$(QEMU) -hda $(OS_BIN)

clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*
