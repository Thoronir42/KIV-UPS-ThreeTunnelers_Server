#include <stdlib.h>

#include "player.h"

player* player_create(int id){
    player* tmp = malloc(sizeof(player));
    tmp->id = id;
    return tmp;
}

void player_delete(player* p){
    free(p);
}