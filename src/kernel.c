#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"

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

int strlen(char *str) {
    int counter = 0;
    while(str[counter] != '\0')
        counter++;

    return counter;
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

void kernel_main() {
    terminal_initialise();
    print("Hello, world!\ntest");

    // Initialise the interrupt table
    idt_init();
}
