#include "string.h"
#include <stddef.h>

char tolower(char s1) {
    if(s1 >= 65 && s1 <= 90) {
        s1 += 32;
    }

    return s1;
}

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

int strnlen_terminator(const char *str, int max, char terminator) {
    int i = 0;
    for(i = 0; i < max; i++) {
        if(str[i] == '\0' || str[i] == terminator) {
            break;
        }
    }

    return i;
}

char *strcpy(char *dest, const char *src) {
    if(dest == NULL || src == NULL) {
        return NULL;
    }

    char *tmp = dest;
    while(*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';

    return tmp;
}

char *strncpy(char *dest, const char *src, int count) {
    if(dest == NULL || src == NULL) {
        return NULL;
    }

    if(count < 0) {
        return NULL;
    }

    int i = 0;
    for(; i < count - 1; i++) {
        if(src[i] == '\0') {
            break;
        }

        dest[i] = src[i];
    }

    dest[i] = '\0';
    return dest;
}

int istrncmp(const char *str1, const char *str2, int n) {
    unsigned char u1, u2;
    while(n-- > 0) {
        u1 = (unsigned char) *str1++;
        u2 = (unsigned char) *str2++;

        if(u1 != u2 && tolower(u1) != tolower(u2)) {
            return u1 - u2;
        }
        if(u1 == '\0') {
            return 0;
        }
    }

    return 0;
}

int strncmp(const char *str1, const char *str2, int n) {
    unsigned char u1, u2;
    while(n-- > 0) {
        u1 = (unsigned char) *str1++;
        u2 = (unsigned char) *str2++;

        if(u1 != u2) {
            return u1 - u2;
        }
        if(u1 == '\0') {
            return 0;
        }
    }

    return 0;
}

int strcmp(const char *str1, const char *str2) {
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);

    if(str1_len != str2_len)
        return -1;

    for(int i = 0; i < str1_len; i++) {
        if(str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
    }

    return 0;
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

