#ifndef TUNNELER_MAP_CHUNK_H
#define TUNNELER_MAP_CHUNK_H

#include "map_block.h"


// 22 * 22 = 529
#define CHUNK_SIZE_MAX 22

typedef struct tunneler_map_chunk
{
    int assigned_player_rid;
    int size;
    block blocks[CHUNK_SIZE_MAX];
} tunneler_map_chunk;

int tunnel_map_chunk_init(tunneler_map_chunk *p, int size);

void tunneler_map_chunk_set_block(tunneler_map_chunk *p, int x, int y, block b);
block tunneler_map_chunk_get_block(tunneler_map_chunk *p, int x, int y);

#endif /* TUNNELER_MAP_CHUNK_H */

