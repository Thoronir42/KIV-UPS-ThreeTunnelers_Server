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
            p->blocks[y * chunk_size + y] = BLOCK_DIRT;
        }
    }
    
    return 0;
}
