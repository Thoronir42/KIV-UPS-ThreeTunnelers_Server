#ifndef TUNNELER_MAP_H
#define TUNNELER_MAP_H

#include <stdlib.h>
#include <stddef.h>

#include "tunneler_map_chunk.h"
#include "map_block.h"

#include "../model/intpoint.h"
#include "../model/intdimension.h"

#define _CHUNK_SIZE 20

#define MAP_MAX_PLAYERS 4

#define MAP_MAX_WIDTH 16
#define MAP_MAX_HEIGHT 16

typedef struct tunneler_map
{
    const int CHUNK_SIZE;
    
    intdimension chunk_dimensions;
    intdimension block_dimensions;

    tunneler_map_chunk chunks[MAP_MAX_WIDTH * MAP_MAX_HEIGHT];

    intpoint bases[MAP_MAX_PLAYERS];
    int bases_size;
} tunneler_map;

void tunneler_map_init(tunneler_map *map, int width, int height, int chunk_size);

tunneler_map_chunk *tunneler_map_get_chunk(tunneler_map *map, int x, int y);



#endif /* TUNNELER_MAP_H */

