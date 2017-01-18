#include <stdlib.h>
#include <string.h>

#include "player.h"

int player_init(player *p, int client_rid) {
    memset(p, 0, sizeof(player));
    p->client_rid = client_rid;
}
