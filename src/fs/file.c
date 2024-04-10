#include "file.h"
#include "config.h"
#include "string/string.h"
#include "disk/disk.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "status.h"
#include "fat/fat16.h"

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

FILE_MODE file_get_mode_by_string(const char *str) {
    FILE_MODE mode = FILE_MODE_INVALID;
    if(strncmp(str, "r", 1) == 0) {
        mode = FILE_MODE_READ;
    }
    else if(strncmp(str, "w", 1) == 0) {
        mode = FILE_MODE_WRITE;
    }
    else if(strncmp(str, "a", 1) == 0) {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}

int fopen(const char *filename, const char *mode_str) {
    int res = 0;
    struct path_root *root_path = pathparser_parse(filename, NULL);
    if(root_path == NULL) {
        res = -EINVARG;
        goto out;
    }

    // Cannot open the root path
    if(root_path->first == NULL) {
        res = -EINVARG;
        goto out;
    }

    // Ensuring the disk exists
    struct disk *disk = disk_get(root_path->drive_no);
    if(disk == NULL) {
        res = -EIO;
        goto out;
    }

    if(disk->filesystem == NULL) {
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if(mode == FILE_MODE_INVALID) {
        res = -EINVARG;
        goto out;
    }

    void *descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if(ISERR(descriptor_private_data)) {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor *desc = NULL;
    res = file_new_descriptor(&desc);
    if(res < 0) {
        goto out;
    }
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

out:
    // fopen should not return negative values
    if(res < 0) {
        res = 0;
    }

    return res;
}


int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd) {
    int res = 0;
    if(size == 0 || nmemb == 0 || fd < 1) {
        res = -EINVARG;
        goto out;
    }

    struct file_descriptor *descriptor = file_get_descriptor(fd);
    if(descriptor == NULL) {
        res = -EINVARG;
        goto out;
    }

    res = descriptor->filesystem->read(descriptor->disk, descriptor->private, size, nmemb, (char *) ptr);

out:
    return res;
}
