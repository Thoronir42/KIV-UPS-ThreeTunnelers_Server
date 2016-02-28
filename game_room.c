#include <stdlib.h>

#include "game_room.h"

game_room* game_room_create(int id){
    game_room* tmp = malloc(sizeof(game_room));
    tmp->id = id;
    return tmp;
}

void game_room_delete(game_room *p_game_room){
    free(p_game_room);
}
