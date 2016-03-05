
#include "tunneler_map_chunk.h"
#include "tunneler_map_block.h"

tunnel_map_chunk *tunnel_map_chunk_create(int chunk_size){
	tunnel_map_chunk *tmp = malloc(sizeof(tunnel_map_chunk));
	
	memset(tmp->block_map, MAP_BLOCK_EARTH, chunk_size * chunk_size);
	
	return tmp;
}

void tunnel_map_chunk_delete(tunnel_map_chunk *p){
	free(p->);
}