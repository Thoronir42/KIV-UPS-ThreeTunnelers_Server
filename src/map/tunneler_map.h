#ifndef TUNNELER_MAP_H
#define TUNNELER_MAP_H

#include <stdlib.h>
#include <stddef.h>

#include "tunneler_map_chunk.h"

#define _CHUNK_SIZE 20

#define MAP_BLOCK_EMPTY 0
#define MAP_BLOCK_EARTH 1
#define MAP_BLOCK_ROCK 2
#define MAP_BLOCK_WALL 3

typedef struct tunneler_map {
	const int CHUNK_SIZE;
	const int CHUNKS_VERTICAL;
	const int CHUNKS_HORITZONTAL;

	int BASE_CHUNK_X[];
	int BASE_CHUNK_Y[];
	
	
	tunneler_map_chunk *chunk_map;

} tunneler_map;

tunneler_map *tunneler_map_create(int width, int height, int chunk_size);

void tunneler_map_delete(tunneler_map *p);

tunneler_map_chunk *tunneler_map_get_chunk(tunneler_map *map, int x, int y);



#endif /* TUNNELER_MAP_H */

