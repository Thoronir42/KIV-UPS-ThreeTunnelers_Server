#include "engine.h"

void _engine_update_game_room(game_room *p_gr){
    
}

void _engine_update_game_rooms(engine *p) {
    int i;
    for (i = 0; i < p->resources->game_rooms_length; i++) {
        _engine_update_game_room(p->resources->game_rooms + 1);
    }
}

void _engine_clean_game_room(engine *p, game_room *p_gr) {
    
}