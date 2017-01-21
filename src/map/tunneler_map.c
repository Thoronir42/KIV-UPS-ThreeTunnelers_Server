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

void tunneler_map_assign_base(tunneler_map *p, int base, int playerRID) {
    tunneler_map_chunk *p_chunk;

    p_chunk = tunneler_map_get_chunk(p, p->bases[base].x, p->bases[base].y);
    tunneler_map_chunk_assing_player(p_chunk, playerRID);

}
