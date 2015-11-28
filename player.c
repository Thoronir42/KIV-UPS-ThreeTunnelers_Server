#include <stdlib.h>

#include "player.h"

player* create_player(int id){
    player* tmp = malloc(sizeof(player));
    tmp->i = id;
    return tmp;
}

void delete_player(player* p){
    free(p);
}