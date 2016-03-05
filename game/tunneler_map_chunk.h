
#ifndef TUNNELER_MAP_CHUNK_H
#define	TUNNELER_MAP_CHUNK_H

#include "tunneler_map_block.h"

typedef struct tunnel_map_chunk {
	byte assigned_player;
	
	byte block_map[][];
	
} tunnel_map_chunk;


tunnel_map_chunk *tunnel_map_chunk_create();

void tunnel_map_chunk_delete(tunnel_map_chunk *p);

#endif	/* TUNNELER_MAP_CHUNK_H */

