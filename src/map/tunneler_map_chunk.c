#include <stdlib.h>
#include <string.h>

#include "tunneler_map_chunk.h"
#include "tunneler_map.h"

int tunnel_map_chunk_init(tunneler_map_chunk *p, tunneler_map *map){
	p->map = map;
	p->blocks = malloc(sizeof(char) * map->CHUNK_BLOCKS);
	memset(p->blocks, MAP_BLOCK_EARTH, map->CHUNK_BLOCKS);
	
	return 0;
}

void tunnel_map_chunk_delete(tunneler_map_chunk *p){
	free(p->blocks);
}