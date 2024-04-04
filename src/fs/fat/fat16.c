#include "fat16.h"
#include "status.h"
#include "string/string.h"
#include <stddef.h>

int fat16_resolve(struct disk *disk);
void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode);

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open
};

struct filesystem *fat16_init() {
    strcpy(fat16_fs.name, "FAT16");

    return &fat16_fs;
}

int fat16_resolve(struct disk *disk) {
    return -EIO;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode) {
    return NULL;
}
