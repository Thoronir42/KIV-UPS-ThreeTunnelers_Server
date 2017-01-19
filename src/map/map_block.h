
#ifndef MAP_BLOCK_H
#define MAP_BLOCK_H

typedef enum {
    BLOCK_EMPTY = 0, BLOCK_DIRT = 1, BLOCK_ROCK = 2, BLOCK_BASE_WALL = 4
} block;

int block_is_obstacle(block b);

int block_is_breakable(block b);

#endif /* MAP_BLOCK_H */

