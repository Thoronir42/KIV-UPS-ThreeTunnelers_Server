
#ifndef TUNNELER_MAP_CHUNK_H
#define	TUNNELER_MAP_CHUNK_H

#include "tunneler_map_block.h"

typedef struct tunnel_map_chunk {
	byte assigned_player;
	
	byte block_map[][];
	
} tunnel_map_chunk;


tunnel_map_chunk *tunnel_map_chunk_create();

#endif	/* TUNNELER_MAP_CHUNK_H */

