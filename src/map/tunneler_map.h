#ifndef TUNNELER_MAP_H
#define	TUNNELER_MAP_H

#include <stdlib.h>
#include <stddef.h>

#include "tunneler_map_chunk.h"

#define _CHUNK_SIZE 20

typedef struct tunneler_map {
	const int CHUNK_SIZE;
	const int CHUNKS_VERTICAL;
	const int CHUNKS_HORITZONTAL;
	
	tunnel_map_chunk **chunk_map;
	
} tunneler_map;

tunneler_map *tunneler_map_create(int width, int height, int chunk_size);

void tunneler_map_delete(tunneler_map *p);




#endif	/* TUNNELER_MAP_H */

