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
