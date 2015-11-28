#include <stdlib.h>

#include "gameroom.h"

player* create_GameRoom(int id){
    GameRoom* tmp = malloc(sizeof(GameRoom);
    tmp->id = id;
    return tmp;
}

void delete_player(player* p){
    free(p);
}
