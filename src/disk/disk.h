#ifndef AETHER_DISK_H
#define DISK_H

typedef unsigned int AETHER_DISK_TYPE;

// Represents a real physical hard disk
#define AETHER_DISK_TYPE_REAL 0

struct disk {
    AETHER_DISK_TYPE type;
    int sector_size;
};

int disk_read_block(struct disk *idisk, unsigned int lba, int total, void *buf);
void disk_search_and_init();
struct disk *disk_get(int index);

#endif
