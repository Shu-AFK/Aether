#include "dstream.h"
#include "config.h"
#include "memory/heap/kheap.h"

struct disk_stream *new_disk_stream(int disk_id) {
    struct disk *disk = disk_get(disk_id);
    if(disk == NULL) {
        return NULL;
    }

    struct disk_stream *stream = kzalloc(sizeof(struct disk_stream));
    if(stream == NULL) {
        return NULL;
    }

    stream->pos = 0;
    stream->disk = disk;
    return stream;
}

int disk_stream_seek(struct disk_stream *stream, int pos) {
    stream->pos = pos;
    return 0;
}

int disk_stream_read(struct disk_stream *stream, void *out, int total) {
    int sector = stream->pos / AETHER_SECTOR_SIZE;
    int offset = stream->pos % AETHER_SECTOR_SIZE;
    char buf[AETHER_SECTOR_SIZE];

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if(res < 0) {
        goto out;
    }

    int total_to_read = total > AETHER_SECTOR_SIZE ? AETHER_SECTOR_SIZE : total;
    for(int i = 0; i < total_to_read; i++) {
        *(char *) out++ = buf[offset + i];
    }

    // Adjust the stream
    stream->pos += total_to_read;
    if(total > AETHER_SECTOR_SIZE) {
        res = disk_stream_read(stream, out, total - AETHER_SECTOR_SIZE);
    }

out:
    return res;
}

void disk_stream_close(struct disk_stream *stream) {
    kfree(stream);
}
