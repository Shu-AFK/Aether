#include "fat16.h"
#include "status.h"
#include "string/string.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
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

static int fat16_init_private(struct disk *disk, struct fat16_private *private) {
    memset(private, 0, sizeof(struct fat16_private));
    private->cluster_read_stream = new_disk_stream(disk->id);
    if(private->cluster_read_stream == NULL) {
        return -ENOMEM;
    }

    private->fat_read_stream = new_disk_stream(disk->id);
    if(private->fat_read_stream == NULL) {
        disk_stream_close(private->cluster_read_stream);
        return -ENOMEM;
    }

    private->directory_stream = new_disk_stream(disk->id);
    if(private->directory_stream == NULL) {
        disk_stream_close(private->cluster_read_stream);
        disk_stream_close(private->fat_read_stream);
        return -ENOMEM;
    }

    return 0;
}

static void fat16_close_streams(struct fat16_private *private) {
    disk_stream_close(private->cluster_read_stream);
    disk_stream_close(private->fat_read_stream);
    disk_stream_close(private->directory_stream);
}

int fat16_sector_to_absolute(struct disk *disk, int sector) {
    return sector * disk->sector_size;
}

int fat16_get_total_items_for_directory(struct disk *disk, uint32_t directory_start_sector) {
    struct fat16_directory_item item;
    struct fat16_directory_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));

    struct fat16_private *fat_private = disk->fs_private;

    int res = 0;
    int i = 0;
    int directory_start_pos = directory_start_sector * disk->sector_size;
    struct disk_stream *stream = fat_private->directory_stream;
    if(disk_stream_seek(stream, directory_start_pos) != AETHER_OK) {
        res = -EIO;
        goto out;
    }

    while(1) {
        if(disk_stream_read(stream, &item, sizeof(item)) != AETHER_OK) {
            res = -EIO;
            goto out;
        }

        if(item.filename[0] == 0x00) { // Done
            break;
        }

        if(item.filename[0] == 0xE5) { // Unused item
            continue;
        }

        i++;
    }
    res = i;

out:
    return res;
}

int fat16_get_root_directory(struct disk *disk, struct fat16_private *private, struct fat16_directory *directory) {
    int res = 0;
    struct fat16_header *primary_header = &private->header.primary_header;
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = private->header.primary_header.root_dir_entries;
    int root_dir_size = (root_dir_entries * sizeof(struct fat16_directory_item));
    int total_sectors = root_dir_size / disk->sector_size;
    if(root_dir_size % disk->sector_size != 0) {
        total_sectors++;
    }

    int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

    struct fat16_directory_item *dir = kzalloc(root_dir_size);
    if(dir == NULL) {
        res = -ENOMEM;
        goto out;
    }

    struct disk_stream *stream = private->directory_stream;
    if(disk_stream_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != AETHER_OK) {
        kfree(dir);
        res = -EIO;
        goto out;
    }

    if(disk_stream_read(stream, dir, root_dir_size) != AETHER_OK) {
        kfree(dir);
        res = -EIO;
        goto out;
    }

    directory->item = dir;
    directory->total = total_items;
    directory->sector_pos = root_dir_sector_pos;
    directory->ending_sector_pos = root_dir_sector_pos + (root_dir_size / disk->sector_size);

out:
    return res;
}

int fat16_resolve(struct disk *disk) {
    int res = 0;
    struct fat16_private *fat_private = kzalloc(sizeof(struct fat16_private));
    if(fat_private == NULL) {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_init_private(disk, fat_private);
    if(res != 0) {
        goto out;
    }

    disk->fs_private = fat_private;
    disk->filesystem = &fat16_fs;

    struct disk_stream *stream = new_disk_stream(disk->id);
    if(stream == NULL) {
        res = -ENOMEM;
        goto out;
    }

    if(disk_stream_read(stream, &fat_private->header, sizeof(fat_private->header)) != AETHER_OK) {
        res = -EIO;
        goto out;
    }

    if(fat_private->header.shared.extended_header.signature != 0x29) {
        fat16_close_streams(fat_private);
        res = -EFSNOTUS;
        goto out;
    }

    if(fat16_get_root_directory(disk, fat_private, &fat_private->root_directory) != AETHER_OK) {
        fat16_close_streams(fat_private);
        res = -EIO;
        goto out;
    }

out:
    if(stream != NULL) {
        disk_stream_close(stream);
    }

    if(res < 0) {
        kfree(fat_private);
        disk->fs_private = NULL;
    }

    return res;
}

void fat16_to_proper_string(char **out, const char *int) {
    while(*in != '\0' && *in != ' ') {
        **out = *in;
        *out++;
        in++;
    }

    if(*in == ' ') {
        **out = '\0';
    }
}

void fat16_get_full_relative_filename(struct fat16_directory_item *item, char *out, int max_len) {
    memset(out, 0x00, max_len);
    char *out_tmp = out;
    fat16_to_proper_string(&out_tmp, (const char *) item->filename);

    if(item->ext[0] != '\0' && item->ext[0] != ' ') {
        *out_tmp++ = '.';
        fat16_to_proper_string(&out_tmp, (const char *) item->ext);
    }
}

struct fat16_directory_item *fat16_clone_directory_item(struct fat16_directory_item *item, int size) {
    struct fat16_directory_item *item_copy = NULL;
    if(size < sizeof(struct fat16_directory_item)) {
        return NULL;
    }

    item_copy = kzalloc(size);
    if(item_copy == NULL) {
        return NULL;
    }
    memcpy(item_copy, item, size);

    return item_copy;
}

struct fat16_directory *fat16_load_fat_directory(struct disk *disk, struct fat16_directory_item *item) {
    int res = 0;
    struct fat16_directory *directory = NULL;
    struct fat_private *fat_private = disk->fs_private;
    if(!(item->attribute & FAT16_FILE_SUBDIRECTORY)) {
        res = -EINVARG;
        goto out;
    }

    directory = kzalloc(sizeof(struct fat16_directory));
    if(directory == NULL) {
        res = -ENOMEM;
        goto out;
    }

    int cluster = fat16_get_first_cluster(item);
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total = total_items;
    int directory_size = directory->total * sizeof(struct fat16_directory_item);
    directory->item = kzalloc(directory_size);
    if(directory->item == NULL) {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
    if(res != AETHER_OK) {
        goto out;
    }

out:
    if(res != AETHER_OK) {
        fat16_free_directory(directory);
    }
    return directory;
}

struct fat16_item *fat16_new_fat_item_for_directory_item(struct disk *disk, struct fat16_directory_item item) {
    struct fat16_item *f_item = kzalloc(sizeof(struct fat16_item));
    if(f_item == NULL) {
        return NULL;
    }

    if(item->attribute & FAT16_FILE_SUBDIRECTORY) {
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT16_ITEM_TYPE_DIRECTORY
    }
    else {
        f_item->type = FAT16_ITEM_TYPE_FILE;
        f_item->item = fat16_clone_directory_item(item, sizeof(struct fat16_directory_item));
    }

    return f_item;
}

struct fat16_item *fat16_find_item_in_directory(struct disk *disk, struct fat16_directory *directory, const char *name) {
    struct fat16_item *f_item = NULL;
    char tmp_filename[AETHER_MAX_PATH];

    for(int i = 0; i < directory->total; i++) {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
        if(istrncmp(tmp_filename, name, sizeof(tmp_filename)) == 0) {
            f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }
}

struct fat16_item *fat16_get_directory_entry(struct disk *disk, struct path_part *path) {
    struct fat16_private *fat_private = disk->fs_private;
    struct fat16_item *current_item = NULL;
    struct fat16_item *root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part);

    if(root_item == NULL) {
        goto out;
    }

out:
    return current_item;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode) {
    if(mode != FILE_MODE_READ) {
        return ERROR(-ERDONLY);
    }

    struct fat16_file_descriptor *descriptor = NULL;
    descriptor = kzalloc(sizeof(struct fat16_file_descriptor));
    if(descriptor == NULL) {
        return ERROR(-ENOMEM);
    }

    descriptor->item = fat16_get_directory_entry(disk, path);
    if(descriptor->item == NULL) {
        return ERROR(-EIO);
    }

    descriptor->pos = 0;
    return descriptor;
}
