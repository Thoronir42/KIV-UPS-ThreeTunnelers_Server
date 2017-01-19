#include "map_generator.h"


void map_generator_generate(tunneler_map *map, int player_count){
    _map_generator_rockify(map);
    _map_generator_plant_bases(map, player_count);
    
}