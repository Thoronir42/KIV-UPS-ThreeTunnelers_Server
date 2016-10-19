#include <stdlib.h>
#include <stddef.h>

#include "tunneler_map.h"

tunneler_map *tunneler_map_create(int width, int height, int chunk_size){
	int x, y;
	
	tunneler_map *tmp = malloc(sizeof(tunneler_map));
	
	*(int *)&tmp->CHUNK_SIZE = chunk_size;
	*(int *)&tmp->CHUNKS_HORITZONTAL = width;
	*(int *)&tmp->CHUNKS_VERTICAL = height;
	
	
	tmp->chunk_map = malloc(sizeof(tunnel_map_chunk*) * width * height);
	
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
