#include <stdlib.h>

#include "player.h"

int player_init(player *p, unsigned short id, unsigned short client_id) {
    p->id = id;
    p->client_aid = client_id;
}
