BIN_DIR := bin
BOOT_SRC := ./src/bootloader/boot.asm
BOOT_BIN := $(BIN_DIR)/boot.bin
QEMU := qemu-system-x86_64

.PHONY: all test clean

all: $(BOOT_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BOOT_BIN): $(BOOT_SRC) | $(BIN_DIR)
	nasm -f bin $< -o $@

test: all
	$(QEMU) -hda $(BOOT_BIN)

clean:
	rm -rf $(BIN_DIR)
