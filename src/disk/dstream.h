#ifndef AETHER_DSTEAM_H
#define AETHER_DSTEAM_H

#include "disk.h"

struct disk_stream {
    int pos;
    struct disk *disk;
};

struct disk_stream *new_disk_stream(int disk_id);
int disk_stream_seek(struct disk_stream *stream, int pos);
int disk_stream_read(struct disk_stream *stream, void *out, int total);
void disk_stream_close(struct disk_stream *stream);

#endif
