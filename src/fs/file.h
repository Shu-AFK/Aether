#ifndef AETHER_FILE_H
#define AETHER_FILE_H

#include "pparser.h"

typedef unsigned int FILE_SEET_MODE;
enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;
enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;
typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct path_part *path, FILE_MODE mode_str);
typedef int(*FS_RESOLVE_FUNCTION)(struct disk *disk);

struct filesystem {
    // Filesystem should return 0 form resolve if the provided disk is using its filesystem
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;

    char name[20];
};

struct file_descriptor {
    // Discriptor index
    int index;
    struct filesystem *filesystem;

    // Private data for internal descriptor
    void *private;

    // Disk of the file descriptor
    struct disk *disk;
};

void fs_init();
int fopen(const char *filename, const char *mode_str);
void fs_insert_filesystem(struct filesystem *filesystem);
struct filesystem *fs_resolve(struct disk *disk);

#endif
