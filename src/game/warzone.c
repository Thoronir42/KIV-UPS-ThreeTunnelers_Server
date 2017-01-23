#include <string.h>

#include "warzone.h"
#include "../generic.h"
#include "../model/direction.h"

void warzone_init(warzone *p, int tanks_size) {
    warzone_rules rules;
    memset(p->tanks, 0, sizeof (tank) * WARZONE_MAX_PLAYERS);
    memset(p->projectiles, 0, sizeof (projectile) * WARZONE_MAX_PROJECTILES);

    p->tanks_size = tanks_size;
    p->projectiles_size = WARZONE_MAX_PROJECTILES;

    rules.TANK_MAX_EP = 250;
    rules.TANK_MAX_HP = 20;
    rules.MAX_PROJECTILES_PER_TANK = WARZONE_PROJECTILES_PER_TANK;
    rules.TANK_CANNON_COOLDOWN = 5;
    rules.TANK_CANNON_COOLDOWN_RATE = 1;
    *(warzone_rules *)&p->rules = rules;
}

tank *warzone_get_tank(warzone *p, int playerRID) {
    if (playerRID < 0 || playerRID >= p->tanks_size) {
        return NULL;
    }

    return p->tanks + playerRID;
}

void warzone_init_tank(warzone *p, int player_rid, intpoint location) {
    tank_init(p->tanks + player_rid, location.x, location.y, DIRECTION_N, 
            p->rules.TANK_MAX_HP, p->rules.TANK_MAX_EP);
}