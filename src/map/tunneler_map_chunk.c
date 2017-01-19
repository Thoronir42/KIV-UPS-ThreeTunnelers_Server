#include <stdlib.h>
#include <string.h>

#include "tunneler_map_chunk.h"
#include "tunneler_map.h"
#include "../generic.h"

int tunnel_map_chunk_init(tunneler_map_chunk *p, int chunk_size) {
    int x, y;

    p->assigned_player_rid = ITEM_EMPTY;
    p->size = chunk_size;
    
    for (y = 0; y < chunk_size; y++) {
        for (x = 0; x < chunk_size; x++) {
            tunneler_map_chunk_set_block(p, x, y, BLOCK_DIRT);
        }
    }
    
    return 0;
}

void tunneler_map_chunk_set_block(tunneler_map_chunk *p, int x, int y, block b){
    p->blocks[y * p->size + x] = b;
}

block tunneler_map_chunk_get_block(tunneler_map_chunk *p, int x, int y){
    return p->blocks[y * p->size + x];
}
