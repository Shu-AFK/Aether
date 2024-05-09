#ifndef AETHER_STRING_H
#define AETHER_STRING_H

#include <stdbool.h>

char tolower(char s1);
int strlen(const char *str);
int strnlen(const char *str, int max);
int strnlen_terminator(const char *str, int max, char terminator);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int count);
int istrncmp(const char *str1, const char *str2, int n);
int strncmp(const char *str1, const char *str2, int n);
int strcmp(const char *str1, const char *str2);
bool is_digit(const char c);
int to_numeric_digit(const char c);
void reverse(char *s);
void itoa(int n, char *s);

#endif
