#ifndef AETHER_PROCESS_H
#define AETHER_PROCESS_H

#include <stdint.h>

#include "task.h"
#include "config.h"

struct process {
    // Process ID
    uint16_t id;

    char filename[AETHER_MAX_PATH];

    // The main process task
    // TODO: A process should have multiple tasks in the future, not just one
    struct task *task;

    // The memory allocations of the process
    // TODO: Make this a linked list or something like that that expands this list dynamically.
    void *allocations[AETHER_MAX_PROGRAM_ALLOCATIONS];

    // The physical pointer to the process memory
    void *ptr;

    // The physical pointer to the stack
    void *stack;

    // The size of the data pointed to by pointer
    uint32_t size;
};

int process_load_for_slot(const char *filename, struct process **process, int process_slot);

#endif
