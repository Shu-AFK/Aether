#ifndef AETHER_STRING_H
#define AETHER_STRING_H

#include <stdbool.h>

int strlen(const char *str);
int strnlen(const char *str, int max);
char *strcpy(char *dest, const char *src);
bool is_digit(const char c);
int to_numeric_digit(const char c);
void reverse(char *s);
void itoa(int n, char *s);

#endif
