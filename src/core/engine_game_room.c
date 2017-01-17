#include "engine.h"

void _engine_update_game_rooms(engine *p) {
    int i;
    for (i = 0; i < p->resources->game_rooms_length; i++) {
        game_room_update(p->resources->game_rooms + 1);
    }
}

void _engine_clean_game_room(engine *p, game_room *p_gr) {
    
}