#include <stdlib.h>
#include <string.h>

#include "tunneler_map_chunk.h"
#include "tunneler_map.h"
#include "../generic.h"

int tunnel_map_chunk_init(tunneler_map_chunk *p, int chunk_size) {
    int x, y;

    p->assigned_player_rid = ITEM_EMPTY;
    p->size = chunk_size;
    p->type = TUNNELER_MAP_CHUNK_TYPE_REGULAR;

    for (y = 0; y < chunk_size; y++) {
        for (x = 0; x < chunk_size; x++) {
            tunneler_map_chunk_set_block(p, x, y, BLOCK_DIRT);
        }
    }

    return 0;
}

void tunneler_map_chunk_set_type(tunneler_map_chunk *p, enum tunneler_map_chunk_type type) {
    p->type = type;
}

void tunneler_map_chunk_assing_player(tunneler_map_chunk *p, int playerRID) {
    p->assigned_player_rid = playerRID;
    p->type = TUNNELER_MAP_CHUNK_TYPE_PLAYER_BASE;
}

int tunneler_map_chunk_is_base(tunneler_map_chunk *p) {
    return p->type == TUNNELER_MAP_CHUNK_TYPE_PLAYER_BASE;
}

void tunneler_map_chunk_set_block(tunneler_map_chunk *p, int x, int y, block b) {
    p->blocks[y * p->size + x] = b;
}

block tunneler_map_chunk_get_block(tunneler_map_chunk *p, int x, int y) {
    return p->blocks[y * p->size + x];
}
