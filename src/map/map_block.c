#include "map_block.h"

int block_is_obstacle(block b) {
    switch (b) {
        case BLOCK_ROCK:
        case BLOCK_BASE_WALL:
        case BLOCK_UNDEFINED:
            return 1;
        default:
            return 0;

    }
    
}

int block_is_breakable(block b) {
    switch (b) {
        case BLOCK_DIRT:
            return 1;
        default:
            return 0;
    }
}
