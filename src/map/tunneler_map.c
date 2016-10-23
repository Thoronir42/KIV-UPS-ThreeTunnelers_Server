#include <stdlib.h>
#include <stddef.h>

#include "tunneler_map.h"

tunneler_map *tunneler_map_create(int width, int height, int chunk_size){
	int x, y;
	
	tunneler_map *tmp = malloc(sizeof(tunneler_map));
	
	*(int *)&tmp->CHUNK_SIZE = chunk_size;
	*(int *)&tmp->CHUNKS_HORITZONTAL = width;
	*(int *)&tmp->CHUNKS_VERTICAL = height;
	
	
	tmp->chunk_map = malloc(sizeof(tunneler_map_chunk*) * width * height);
	
	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			tmp->chunk_map[y * width + x] = tunnel_map_chunk_create(tmp->CHUNK_SIZE);
		}
	}
	
	return tmp;
}

void tunneler_map_delete(tunneler_map *p){
	int x, y;
	for(y = 0; y < p->CHUNKS_VERTICAL; y++){
		for(x = 0; x < p->CHUNKS_HORITZONTAL; x++){
			tunnel_map_chunk_delete(p->chunk_map[y * p->CHUNKS_HORITZONTAL + x]);
		}
	}
	free(p->chunk_map);
	
	free(p);
}

tunneler_map_chunk *tunneler_map_get_chunk(tunneler_map *map, int x, int y){
	int offset;
	
	if(y < 0 || y > map->CHUNKS_VERTICAL || x < 0 || x > map->CHUNKS_HORITZONTAL){
		return NULL;
	}
	offset = y * map->CHUNKS_HORITZONTAL + x;
	
	return map.chunk_map + offset;
}
