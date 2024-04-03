#include "string.h"

int strlen(const char *str) {
    int counter = 0;
    while(str[counter] != '\0')
        counter++;

    return counter;
}

int strnlen(const char *str, int max) {
    int i;
    for(i = 0; i < max; i++) {
        if(str[i] == '\0') {
            break;
        }
    }

    return i;
}

bool is_digit(const char c) {
    return c >= 48 && c <= 57;
}

int to_numeric_digit(const char c) {
    return c - 48;
}

void reverse(char *s) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

 void itoa(int n, char *s) {
     int i, sign;

     if ((sign = n) < 0)
         n = -n;
     i = 0;

     do {
         s[i++] = n % 10 + '0';
     } while ((n /= 10) > 0);

     if (sign < 0)
         s[i++] = '-';

     s[i] = '\0';
     reverse(s);
 }

