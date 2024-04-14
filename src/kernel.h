#ifndef AETHER_KERNEL_H
#define AETHER_KERNEL_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void kernel_main();
int print(const char *str);
int print_col(const char *str, const char color);
void panic(const char *msg);

#define ERROR(value) (void *) (value)
#define ERROR_I(value) (int) (value)
#define ISERR(value) ((int) value < 0)

#endif
