#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "memory/memory.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "disk/dstream.h"
#include "fs/file.h"
#include "config.h"
#include "gdt/gdt.h"
#include "task/tss.h"

uint16_t *video_mem;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(const char c, const char color) {
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, const char c, const char color) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(const char c, const char color) {
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

int print(const char *str) {
    int len = strlen(str);

    for(int i = 0; i < len; i++) {
        terminal_writechar(str[i], 15);
    }

    return 0;
}

int print_col(const char *str, const char color) {
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

struct tss tss;

struct gdt gdt_real[AETHER_TOTAL_GDT_SEGMENTS];

struct gdt_structured gdt_structured[AETHER_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00,              .limit = 0x00,          .type = 0x00},        // NULL segment
    {.base = 0x00,              .limit = 0xffffffff,    .type = 0x9a},        // Kernel code segment
    {.base = 0x00,              .limit = 0xffffffff,    .type = 0x92},        // Kernel data segment
    {.base = 0x00,              .limit = 0xffffffff,    .type = 0xf8},        // User code segment
    {.base = 0x00,              .limit = 0xffffffff,    .type = 0xf2},        // User data segment
    {.base = (uint32_t) &tss,   .limit = sizeof(tss),   .type = 0xE9},        // TSS segment
};

void kernel_main() {
    terminal_initialise();

    print("Hello, world!\n");

    // Sets and loads the global descriptor table
    memset(gdt_real, 0, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, AETHER_TOTAL_GDT_SEGMENTS);

    gdt_load(gdt_real, sizeof(gdt_real));

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

    // Setup TSS
    memset(&tss, 0, sizeof(tss));
    tss.esp0 = 0x600000; // Kernel stack location
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the tss
    tss_load(0x28); // Offset in the gdt_real for the tss segment

    // Sets up paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk)); // Switch to kernel paging chunk
    enable_paging();


    enable_interrupts();

    while(1) {}
}
