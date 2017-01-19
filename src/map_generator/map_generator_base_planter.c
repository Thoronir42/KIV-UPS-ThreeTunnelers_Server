#include "map_generator.h"
#include "../map/tunneler_map.h"
#include "../model/intdimension.h"

void _map_generator_plant_base_on_chunk(tunneler_map_chunk *chunk, int chunkSize) {
    int padding = 3, row, col, i;
    int top = padding, left = top, bottom = chunkSize - padding, right = bottom;
    int width, start, end, orientation;

    for (row = top; row <= bottom; row++) {
        tunneler_map_chunk_set_block(chunk, left, row, BLOCK_BASE_WALL);
        tunneler_map_chunk_set_block(chunk, right, row, BLOCK_BASE_WALL);
    }
    for (col = left; col <= right; col++) {
        tunneler_map_chunk_set_block(chunk, col, top, BLOCK_BASE_WALL);
        tunneler_map_chunk_set_block(chunk, col, bottom, BLOCK_BASE_WALL);
    }

    for (row = top + 1; row < bottom; row++) {
        for (col = left + 1; col < right; col++) {
            tunneler_map_chunk_set_block(chunk, col, row, BLOCK_EMPTY);
        }
    }

    width = (chunkSize % 2) == 1 ? 7 : 6;

    start = (chunkSize - width) / 2;
    end = (chunkSize + width) / 2;
    orientation = rand() & (2);
    for (i = start; i <= end; i++) {
        if (orientation == 0) {
            tunneler_map_chunk_set_block(chunk, left, i, BLOCK_EMPTY);
            tunneler_map_chunk_set_block(chunk, right, i, BLOCK_EMPTY);
        } else {
            tunneler_map_chunk_set_block(chunk, i, top, BLOCK_EMPTY);
            tunneler_map_chunk_set_block(chunk, i, bottom, BLOCK_EMPTY);
        }
    }
}

void _map_generator_plant_bases(tunneler_map* map, int count) {
    int i, x, y;
    tunneler_map_chunk *p_chunk;
    intdimension dimensions = map->chunk_dimensions;

    // TODO: base distance
    // TODO: terrain editing
    for (i = 0; i < count; i++) {
        do {
            x = rand() % (dimensions.width - 2) + 1;
            y = rand() % (dimensions.height - 2) + 1;
            p_chunk = tunneler_map_get_chunk(map, x, y);
        } while (tunneler_map_chunk_is_base(p_chunk));

        _map_generator_plant_base_on_chunk(p_chunk, p_chunk->size);
        
        map->bases[i].x = x;
        map->bases[i].y = y;
    }
}
