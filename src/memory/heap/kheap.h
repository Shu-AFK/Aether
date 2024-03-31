#ifndef AETHER_KHEAP_H
#define AETHER_KHEAP_H

#include <stdint.h>
#include <stddef.h>

void *kmalloc(size_t size);
int kheap_init();
void kfree(void *addr);

#endif
