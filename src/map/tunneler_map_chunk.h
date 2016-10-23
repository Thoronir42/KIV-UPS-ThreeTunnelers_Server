
#ifndef TUNNELER_MAP_CHUNK_H
#define	TUNNELER_MAP_CHUNK_H

typedef struct tunneler_map_chunk {
	short assigned_player;
	
	char *block_map;
	
} tunneler_map_chunk;


tunneler_map_chunk *tunnel_map_chunk_create();

void tunnel_map_chunk_delete(tunneler_map_chunk *p);

#endif	/* TUNNELER_MAP_CHUNK_H */

