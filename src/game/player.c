#include <stdlib.h>

#include "player.h"

int player_init(player *p, unsigned short client_rid) {
    p->client_rid = client_rid;
}
