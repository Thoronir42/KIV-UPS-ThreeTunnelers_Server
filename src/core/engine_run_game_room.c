#include <string.h>

#include "engine.h"

#include "../logger.h"
#include "../game/warzone.h"

void _engine_update_gameroom_starting(engine *p, game_room *p_gr) {
    int i;
    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->clients[i] == NULL) {
            continue;
        }
        if (p_gr->clients[i]->connection == NULL) {
            network_command_prepare(p->p_cmd_out, NCT_MSG_SYSTEM);
            network_command_append_str(p->p_cmd_out, "Some clients crashed"
                    ", returning to lobby");
            engine_game_room_set_state(p, p_gr, GAME_ROOM_STATE_LOBBY);
        }
    }
}

void _engine_update_gameroom_battle(engine *p, game_room *p_gr) {
    int i;
    warzone *p_wz = &p_gr->zone;
    for (i = 0; i < p_wz->tanks_size; i++) {

    }
}

void engine_update_gamerooms(engine *p) {
    int i;
    game_room *p_gr;
    void(* update_action)(engine *, game_room *);
    int tickskip;

    for (i = 0; i < p->resources->game_rooms_length; i++) {
        p_gr = p->resources->game_rooms + i;
        if (p_gr->state < 0 || p_gr->state >= GAME_ROOM_STATES_COUNT) {
            glog(LOG_WARNING, "Game room %d is in invalid state.", i);
            engine_game_room_cleanup(p, p_gr);
            continue;
        }
        update_action = p->game_room_update_functions[p_gr->state];
        if (update_action == NULL) {
            continue;
        }
        tickskip = p->game_room_update_tickskip[p_gr->state];
        if ((p->current_tick + p_gr->state) % tickskip == 0) {
            update_action(p, p_gr);
        }
    }
}

void _engine_init_gameroom_updates(engine *p) {
    memset(p->game_room_update_functions, 0, GAME_ROOM_STATES_COUNT * sizeof (void(*)(engine*, game_room*)));
    memset(p->game_room_update_tickskip, 0, GAME_ROOM_STATES_COUNT * sizeof (int));

    p->game_room_update_functions[GAME_ROOM_STATE_BATTLE_STARTING] = &_engine_update_gameroom_starting;
    p->game_room_update_tickskip[GAME_ROOM_STATE_BATTLE_STARTING] = 16;

    p->game_room_update_functions[GAME_ROOM_STATE_BATTLE] = &_engine_update_gameroom_battle;
    p->game_room_update_tickskip[GAME_ROOM_STATE_BATTLE] = 1;
    p->game_room_update_functions[GAME_ROOM_STATE_SUMMARIZATION] = &_engine_update_gameroom_battle;
    p->game_room_update_tickskip[GAME_ROOM_STATE_SUMMARIZATION] = 1;
}
