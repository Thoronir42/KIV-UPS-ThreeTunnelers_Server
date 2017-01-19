
#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "../map/tunneler_map.h"

void map_generator_generate(tunneler_map *map, int player_count);

void _map_generator_rockify(tunneler_map *map);

void _map_generator_plant_bases(tunneler_map* map, int count);

#endif /* MAP_GENERATOR_H */

