#ifndef TUNNELER_MAP_H
#define	TUNNELER_MAP_H

#include <stdlib.h>
#include <stddef.h>

#include "game/tunneler_map_chunk.h"

typedef struct tunneler_map {
	const int CHUNK_SIZE = 20;
	const int CHUNKS_VERTICAL;
	const int CHUNKS_HORITZONTAL;
	
	tunnel_map_chunk *chunk_map[][];
	
} tunneler_map;

tunneler_map *tunneler_map_create(int width, int height);

void tunneler_map_create(tunneler_map *p);




#endif	/* TUNNELER_MAP_H */

