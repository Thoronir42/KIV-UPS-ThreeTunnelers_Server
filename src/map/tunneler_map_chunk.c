#include <stdlib.h>
#include <string.h>

#include "tunneler_map_chunk.h"
#include "tunneler_map.h"

tunneler_map_chunk *tunnel_map_chunk_create(int chunk_size){
	tunneler_map_chunk *tmp = malloc(sizeof(tunneler_map_chunk));
	tmp->block_map = malloc(sizeof(char) * chunk_size * chunk_size);
	memset(tmp->block_map, MAP_BLOCK_EARTH, chunk_size * chunk_size);
	
	return tmp;
}

void tunnel_map_chunk_delete(tunneler_map_chunk *p){
	free(p);
}