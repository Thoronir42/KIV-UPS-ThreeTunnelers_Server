
#include "tunneler_map.h"

tunneler_map *tunneler_map_create(int width, int height){
	int x, y;
	
	tunneler_map *tmp = malloc(sizeof(tunneler_map));
	
	tmp->chunk_map = malloc(sizeof(tunnel_map_chunk*) * width * height);
	
	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			tmp->chunk_map[y][x] = tunnel_map_chunk_create();
		}
	}
	
	return tmp;
}
