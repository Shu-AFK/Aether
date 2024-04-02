#ifndef AETHER_STRING_H
#define AETHER_STRING_H

#include <stdbool.h>

int strlen(const char *str);
int strnlen(const char *str, int max);
bool is_digit(const char c);
int to_numeric_digit(const char c);

#endif
