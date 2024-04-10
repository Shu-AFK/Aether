#ifndef AETHER_KERNEL_H
#define AETHER_KERNEL_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void kernel_main();
int print(char *str);
int print_col(char *str, char color);

#define ERROR(value) (void *) (value)
#define ERROR_I(value) (int) (value)
#define ISERR(value) ((int) value < 0)

#endif
