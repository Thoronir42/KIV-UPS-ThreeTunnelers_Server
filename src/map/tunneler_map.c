#include <stdlib.h>
#include <stddef.h>

#include "tunneler_map.h"
#include "tunneler_map_chunk.h"
#include "../model/intdimension.h"

void tunneler_map_init(tunneler_map *map, int width, int height, int chunk_size) {
    int x, y, i;
    tunneler_map_chunk *p_chunk;

    *(int *) &map->CHUNK_SIZE = chunk_size > CHUNK_SIZE_MAX ? CHUNK_SIZE_MAX : chunk_size;
    map->chunk_dimensions.width = width > MAP_MAX_WIDTH ? MAP_MAX_WIDTH : width;
    map->chunk_dimensions.height = height > MAP_MAX_HEIGHT ? MAP_MAX_HEIGHT : height;

    map->block_dimensions.width = map->chunk_dimensions.width * map->CHUNK_SIZE;
    map->block_dimensions.height = map->chunk_dimensions.height * map->CHUNK_SIZE;

    map->bases_size = MAP_MAX_PLAYERS;

    for (y = 0; y < map->chunk_dimensions.height; y++) {
        for (x = 0; x < map->chunk_dimensions.width; x++) {
            p_chunk = tunneler_map_get_chunk(map, x, y);
            tunnel_map_chunk_init(p_chunk, map->CHUNK_SIZE);
        }
    }
    for (i = 0; i < map->bases_size; i++) {
        map->bases[i] = (intpoint){0, 0};
    }
}

tunneler_map_chunk *tunneler_map_get_chunk(tunneler_map *map, int x, int y) {
    if (y < 0 || y > map->chunk_dimensions.height || x < 0 || x > map->chunk_dimensions.width) {
        return NULL;
    }

    return map->chunks + (y * map->chunk_dimensions.width + x);
}

intpoint tunneler_map_assign_base(tunneler_map *p, int n, int playerRID) {
    tunneler_map_chunk *p_chunk;
    intpoint base, center;

    base = p->bases[n];

    p_chunk = tunneler_map_get_chunk(p, base.x, base.y);
    tunneler_map_chunk_assing_player(p_chunk, playerRID);

    center.x = base.x * p->CHUNK_SIZE + p->CHUNK_SIZE / 2;
    center.y = base.y * p->CHUNK_SIZE + p->CHUNK_SIZE / 2;

    return center;
}

block tunneler_map_get_block(tunneler_map *p, int x, int y) {
    if(x < 0 || x > p->block_dimensions.width ||
            y < 0 || y > p->block_dimensions.height){
        return BLOCK_UNDEFINED;
    }
    tunneler_map_chunk *p_chunk = tunneler_map_get_chunk(p,
            x / p->CHUNK_SIZE, y / p->CHUNK_SIZE);
    return tunneler_map_chunk_get_block(p_chunk,
            x % p->CHUNK_SIZE, y % p->CHUNK_SIZE);
}

int tunneler_map_set_block(tunneler_map *p, int x, int y, block b) {
    block current = tunneler_map_get_block(p, x, y);
    if (current == b) {
        return 0;
    }
    tunneler_map_chunk *p_chunk = tunneler_map_get_chunk(p,
            x / p->CHUNK_SIZE, y / p->CHUNK_SIZE);
    tunneler_map_chunk_set_block(p_chunk,
            x % p->CHUNK_SIZE, y % p->CHUNK_SIZE, b);
    
    return 1;
}
