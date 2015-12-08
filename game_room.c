#include <stdlib.h>

#include "game_room.h"

player* create_GameRoom(int id){
    GameRoom* tmp = malloc(sizeof(GameRoom);
    tmp->id = id;
    return tmp;
}

void delete_player(player* p){
    free(p);
}
