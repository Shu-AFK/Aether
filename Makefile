# Directory Variables
BIN_DIR := bin
BUILD_DIR := build
BUILD_DIR_MEM := $(BUILD_DIR)/memory
BUILD_DIR_IDT := $(BUILD_DIR)/idt
BUILD_DIR_IO := $(BUILD_DIR)/io
BUILD_DIR_DISK := $(BUILD_DIR)/disk
BUILD_DIR_STR := $(BUILD_DIR)/string
BUILD_DIR_FS := $(BUILD_DIR)/fs
BUILD_DIR_GDT := $(BUILD_DIR)/gdt
BUILD_DIR_TASK := $(BUILD_DIR)/task
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
IO_ASM_OBJ := $(BUILD_DIR_IO)/io.asm.o
IO_ASM_SRC := ./src/io/io.asm
HEAP_OBJ := $(BUILD_DIR_MEM)/heap/heap.o
HEAP_SRC := ./src/memory/heap/heap.c
KHEAP_OBJ := $(BUILD_DIR_MEM)/heap/kheap.o
KHEAP_SRC := ./src/memory/heap/kheap.c
PAGING_OBJ := $(BUILD_DIR_MEM)/paging/paging.o
PAGING_SRC := ./src/memory/paging/paging.c
PAGING_ASM_OBJ := $(BUILD_DIR_MEM)/paging/paging.asm.o
PAGING_ASM_SRC := ./src/memory/paging/paging.asm
DISK_OBJ := $(BUILD_DIR_DISK)/disk.o
DISK_SRC := ./src/disk/disk.c
PARS_OBJ := $(BUILD_DIR_FS)/pparser.o
PARS_SRC := ./src/fs/pparser.c
STR_OBJ := $(BUILD_DIR_STR)/string.o
STR_SRC := ./src/string/string.c
DSTREAM_OBJ := $(BUILD_DIR_DISK)/dstream.o
DSTREAM_SRC := ./src/disk/dstream.c
FILE_OBJ := $(BUILD_DIR_FS)/file.o
FILE_SRC := ./src/fs/file.c
FAT16_OBJ := $(BUILD_DIR_FS)/fat/fat16.o
FAT16_SRC := ./src/fs/fat/fat16.c
GDT_OBJ := $(BUILD_DIR_GDT)/gdt.o
GDT_SRC := ./src/gdt/gdt.c
GDT_ASM_OBJ := $(BUILD_DIR_GDT)/gdt.asm.o
GDT_ASM_SRC := ./src/gdt/gdt.asm
TSS_ASM_OBJ := $(BUILD_DIR_TASK)/tss.asm.o
TSS_ASM_SRC := ./src/task/tss.asm
TASK_OBJ := $(BUILD_DIR_TASK)/task.o
TASK_SRC := ./src/task/task.c

LINKER := ./src/linker.ld
FILES = $(KERNEL_OBJ) $(KERNEL_C_OBJ) $(IDT_ASM_OBJ) $(IDT_OBJ) $(MEM_OBJ) $(IO_ASM_OBJ) $(HEAP_OBJ) $(KHEAP_OBJ) $(PAGING_ASM_OBJ) $(PAGING_OBJ) $(DISK_OBJ) $(STR_OBJ) $(PARS_OBJ) $(DSTREAM_OBJ) $(FAT16_OBJ) $(FILE_OBJ) $(GDT_OBJ) $(GDT_ASM_OBJ) $(TSS_ASM_OBJ) $(TASK_OBJ)

FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

# Tool Variables
QEMU := qemu-system-x86_64

.PHONY: all test clean setup

all: setup $(BOOT_BIN) $(KERNEL_BIN)
	rm -rf $(OS_BIN)
	dd if=$(BOOT_BIN) >> $(OS_BIN)
	dd if=$(KERNEL_BIN) >> $(OS_BIN)
	dd if=/dev/zero bs=1048576 count=16 >> $(OS_BIN) # Fills the rest with 0 to for a sector
	sudo mount -t vfat ./bin/os.bin /mnt/d # Mounts os.bin to /mnt/d so we can store files in the binary
	sudo cp ./test.txt /mnt/d # Copying a testfile to /mnt/d (Remove if you don't want it)
	sudo umount /mnt/d # Unmounting

setup:
	mkdir -p $(BIN_DIR) $(BUILD_DIR) $(BUILD_DIR_MEM) $(BUILD_DIR_IDT) $(BUILD_DIR_IO) $(BUILD_DIR_MEM)/heap $(BUILD_DIR_MEM)/paging $(BUILD_DIR_DISK) $(BUILD_DIR_FS) $(BUILD_DIR_STR) $(BUILD_DIR_FS)/fat $(BUILD_DIR_GDT) $(BUILD_DIR_TASK)

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

$(IO_ASM_OBJ): $(IO_ASM_SRC)
	nasm -f elf -g $< -o $@

$(HEAP_OBJ): $(HEAP_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(KHEAP_OBJ): $(KHEAP_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(PAGING_OBJ): $(PAGING_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(PAGING_ASM_OBJ): $(PAGING_ASM_SRC)
	nasm -f elf -g $< -o $@

$(DISK_OBJ): $(DISK_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(STR_OBJ): $(STR_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(PARS_OBJ): $(PARS_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(DSTREAM_OBJ): $(DSTREAM_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(FILE_OBJ): $(FILE_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(FAT16_OBJ): $(FAT16_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(GDT_OBJ): $(GDT_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

$(GDT_ASM_OBJ): $(GDT_ASM_SRC)
	nasm -f elf -g $< -o $@

$(TSS_ASM_OBJ): $(TSS_ASM_SRC)
	nasm -f elf -g $< -o $@

$(TASK_OBJ): $(TASK_SRC)
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c $< -o $@

test: all
	$(QEMU) -hda $(OS_BIN)

clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*
