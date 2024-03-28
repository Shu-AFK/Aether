#include "memory.h"

void *memset(void *ptr, char c, size_t size) {
    char *c_ptr = (char*) ptr;
    for(size_t i = 0; i < size; i++) {
        c_ptr[i] = (char) c;
    }

    return ptr;
}
