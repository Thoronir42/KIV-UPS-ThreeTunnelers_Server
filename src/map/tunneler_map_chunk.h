#ifndef TUNNELER_MAP_CHUNK_H
#define TUNNELER_MAP_CHUNK_H

#include "map_block.h"


// 22 * 22 = 484
#define CHUNK_SIZE_MAX 22

enum tunneler_map_chunk_type
{
    TUNNELER_MAP_CHUNK_TYPE_REGULAR = 0, 
    TUNNELER_MAP_CHUNK_TYPE_PLAYER_BASE = 1,
};

typedef struct tunneler_map_chunk
{
    int size;
    block blocks[CHUNK_SIZE_MAX * CHUNK_SIZE_MAX];
    
    enum tunneler_map_chunk_type type;
    int assigned_player_rid;
    
} tunneler_map_chunk;

int tunnel_map_chunk_init(tunneler_map_chunk *p, int size);

void tunneler_map_chunk_set_block(tunneler_map_chunk *p, int x, int y, block b);
block tunneler_map_chunk_get_block(tunneler_map_chunk *p, int x, int y);

#endif /* TUNNELER_MAP_CHUNK_H */

