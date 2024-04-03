#include "file.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "status.h"

struct filesystem *filesystems[AETHER_MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[AETHER_MAX_FILE_DESCRIPTORS];

static struct filesystem **fs_get_free_filesystem() {
    for(int i = 0; i < AETHER_MAX_FILESYSTEMS; i++) {
        if(filesystems[i] == 0) {
            return &filesystems[i];
        }
    }

    return NULL;
}

void fs_insert_filesystem(struct filesystem *filesystem) {
    struct filesystem **fs;
    fs = fs_get_free_filesystem();
    if(fs == NULL) {
        // Has to panic in the future
        print("Problem inserting filesystem");
        while(1) {}
    }

    *fs = filesystem;
}

struct filesystem *fat16_init() {
    return NULL;
}

static void fs_static_load() {
    fs_insert_filesystem(fat16_init());
}

void fs_load() {
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor **desc_out) {
    int res = -ENOMEM;
    for(int i = 0; i < AETHER_MAX_FILE_DESCRIPTORS; i++) {
        if(file_descriptors[i] == NULL) {
            struct file_descriptor *desc = kzalloc(sizeof(struct file_descriptor));
            if(desc == NULL) {
                break;
            }

            // Descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor *file_get_descriptor(int fd) {
    if(fd <= 0 || fd >= AETHER_MAX_FILE_DESCRIPTORS) {
        return NULL;
    }

    int index = fd - 1;
    return file_descriptors[index];
}

struct filesystem *fs_resolve(struct disk *disk) {
    struct filesystem *fs = NULL;
    for(int i = 0; i < AETHER_MAX_FILESYSTEMS; i++) {
        if(filesystems[i] != NULL && filesystems[i]->resolve(disk) == 0) {
            fs = filesystems[i];
        }
    }

    return fs;
}

int fopen(const char *filename, const char *mode) {
    return -EIO;
}
