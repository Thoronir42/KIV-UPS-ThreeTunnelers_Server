#include "map_generator.h"
#include "smooth_line.h"
#include "../map/tunneler_map.h"



void _map_generator_rockify_top(tunneler_map_chunk *chunk, smooth_line *line, int chunkSize) {
    int padding, i, offset;
    for (i = 0; i < chunkSize; i++) {
        padding = smooth_line_put(line, rand() % chunkSize);
        for (offset = 0; offset < padding; offset++) {
            tunneler_map_chunk_set_block(chunk, i, offset, BLOCK_ROCK);
        }
    }
}

void _map_generator_rockify_bottom(tunneler_map_chunk *chunk, smooth_line *line, int chunkSize) {
    int padding, i, offset;
    for (i = 0; i < chunkSize; i++) {
        padding = smooth_line_put(line, rand() %chunkSize);
        for (offset = 0; offset < padding; offset++) {
            tunneler_map_chunk_set_block(chunk, i, chunkSize - offset - 1, BLOCK_ROCK);
        }
    }
}

void _map_generator_rockify_left(tunneler_map_chunk *chunk, smooth_line *line, int chunkSize) {
    int padding, i, offset;
    for (i = 0; i < chunkSize; i++) {
        padding = smooth_line_put(line, rand() % chunkSize);
        for (offset = 0; offset < padding; offset++) {
            tunneler_map_chunk_set_block(chunk, offset, i, BLOCK_ROCK);
        }
    }
}

void _map_generator_rockify_right(tunneler_map_chunk *chunk, smooth_line *line, int chunkSize) {
    int padding, i, offset;
    for (i = 0; i < chunkSize; i++) {
        padding = smooth_line_put(line, rand() % chunkSize);
        for (offset = 0; offset < padding; offset++) {
            tunneler_map_chunk_set_block(chunk, chunkSize - offset - 1, i, BLOCK_ROCK);
        }
    }
}


void _map_generator_rockify(tunneler_map *map){
    intdimension dimensions = map->chunk_dimensions;
    int chunkSize = map->CHUNK_SIZE, i;
    tunneler_map_chunk *p_chunk;
    
    smooth_line l1, l2;
    smooth_line_init(&l1, chunkSize / 2);
    smooth_line_init(&l2, chunkSize / 2);

    for (i = 0; i < dimensions.width; i++) {
        p_chunk = tunneler_map_get_chunk(map, i, 0);
        _map_generator_rockify_top(p_chunk, &l1, chunkSize);
        
        p_chunk = tunneler_map_get_chunk(map, i, dimensions.height - 1);
        _map_generator_rockify_bottom(p_chunk, &l1, chunkSize);
    }

    smooth_line_init(&l1, chunkSize / 2);
    smooth_line_init(&l2, chunkSize / 2);

    for (i = 0; i < dimensions.height; i++) {
        p_chunk = tunneler_map_get_chunk(map, 0, i);
        _map_generator_rockify_left(p_chunk, &l1, chunkSize);
        
        p_chunk = tunneler_map_get_chunk(map, dimensions.width - 1, i);
        _map_generator_rockify_right(p_chunk, &l2, chunkSize);
    }
}
