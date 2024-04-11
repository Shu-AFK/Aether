#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "disk/dstream.h"
#include "fs/file.h"

uint16_t *video_mem;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color) {
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color) {
    if(c == '\n') {
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);

    terminal_col++;
    if(terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
}

void terminal_initialise() {
    video_mem = (uint16_t *)(0xB8000);
    terminal_col = 0;
    terminal_row = 0;

    for(int i = 0; i < VGA_HEIGHT; i++) {
        for(int j = 0; j < VGA_WIDTH; j++) {
            video_mem[(i * VGA_WIDTH) + j] = terminal_make_char(' ', 0);
        }
    }
}

int print(char *str) {
    int len = strlen(str);

    for(int i = 0; i < len; i++) {
        terminal_writechar(str[i], 15);
    }

    return 0;
}

int print_col(char *str, char color) {
    int len = strlen(str);

    for(int i = 0; i < len; i++) {
        terminal_writechar(str[i], color);
    }

    return 0;
}

static struct paging_4gb_chunk* kernel_chunk = NULL;

void panic(const char *msg) {
    print(msg);
    while(1) {}
}

void kernel_main() {
    terminal_initialise();

    print("Hello, world!\n");

    // Initialises the heap
    if(kheap_init() < 0) {
        panic("Failed to init heap\n");
    }

    // Initialises filesystems
    fs_init();

    // Search and initialises disks
    disk_search_and_init();

    // Initialises the interrupt descriptor table
    idt_init();

    // Sets up paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk)); // Switch to kernel paging chunk
    enable_paging();


    enable_interrupts();

    int fd = fopen("0:/test.txt", "r");
    if(fd) {
        print("test.txt is open!\n");

        struct file_stat s;
        fstat(fd, &s);

        char buf[200];
        fseek(fd, 2, SEEK_SET);
        fread(buf, 200, 1, fd);
        print(buf);
        fclose(fd);
    } else {
        print("Unabe to find file test.txt!\n");
    }

    while(1) {}
}
