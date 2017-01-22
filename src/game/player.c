#include "player.h"
#include "../generic.h"

int player_init(player *p, int client_rid, int color) {
    p->client_rid = client_rid;
    p->color = color;
    input_set_state(&p->input, 0);
}

void player_cleanup(player *p){
    player_init(p, ITEM_EMPTY, ITEM_EMPTY);
}
