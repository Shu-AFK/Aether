#include "task.h"
#include "status.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"

// Task thats currently running
struct task *current_task = NULL;

struct task *task_tail = NULL;
struct task *task_head = NULL;

int task_init(struct task *task, struct process *process);

struct task *task_current() {
    return current_task;
}

struct task *task_new(struct process *process) {
    int res = 0;
    struct task *task = kzalloc(sizeof(struct task));
    if(task == NULL) {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
    if(res != AETHER_OK) {
        goto out;
    }

    if(task_head == NULL) {
        task_head = task;
        task_tail = task;
        current_task = task;
        goto out;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

out:
    if(ISERR(res)) {
        task_free(task);
        return ERROR(res);
    }

    return task;
}

struct task *task_get_next() {
    if(current_task->next == NULL) {
        return task_head;
    }

    return current_task->next;
}

static void task_list_remove(struct task *task) {
    if(task->prev != NULL) {
        task->prev->next = task->next;
    }

    if(task == task_head) {
        task_head = task->next;
    }

    if(task == task_tail) {
        task_tail = task->prev;
    }

    if(task == current_task) {
        current_task = task_get_next();
    }
}

int task_free(struct task *task) {
    paging_free_4gb(task->page_directory);
    task_list_remove(task);

    kfree(task);
    return 0;
}

int task_switch(struct task *task) {
    current_task = task;
    paging_switch(task->page_directory->directory_entry);
    return 0;
}

int task_page() {
    user_registers();
    task_switch(current_task);
    return 0;
}

void run_first_task() {
    if(current_task == NULL) {
        panic("run_first_task(): No current task exists!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
}

int task_init(struct task *task, struct process *process) {
    memset(task, 0, sizeof(struct task));

    // Map the entire 4gb address space to itself
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if(!task->page_directory) {
        return -EIO;
    }

    task->registers.ip = AETHER_PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = AETHER_PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    task->process = process;

    return 0;
}
