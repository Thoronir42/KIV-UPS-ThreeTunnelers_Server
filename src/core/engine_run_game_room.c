#include <string.h>

#include "engine.h"

#include "../logger.h"
#include "../game/warzone.h"
#include "../game/entity_shape.h"

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

int _warzone_can_tank_move_to(warzone *p_wz, int new_x, int new_y,
        shape shape_belt, shape shape_body) {
    int sx, sy;
    block b;
    for (sy = shape_belt.min.y; sy <= shape_belt.max.y; sy++) {
        for (sx = shape_belt.min.x; sx <= shape_belt.max.x; sx++) {
            if (!shape_is_solid_o(shape_belt, sx, sy) &&
                    !shape_is_solid_o(shape_body, sx, sy)) {
                continue;
            }

            b = tunneler_map_get_block(&p_wz->map, new_x + sx, new_y + sy);
            if (block_is_obstacle(b)) {
                return 0;
            }
        }
    }

    return 1;
}

void _warzone_break_blocks_around(warzone *p_wz, int new_x, int new_y,
        shape shape_belt, shape shape_body) {
    int sx, sy, belt_pixel, body_pixel;
    block b;
    for (sy = shape_belt.min.y; sy <= shape_belt.max.y; sy++) {
        for (sx = shape_belt.min.x; sx <= shape_belt.max.x; sx++) {
            body_pixel = shape_is_solid_o(shape_body, sx, sy);
//            belt_pixel = shape_is_solid_o(shape_belt, sx, sy);

            if (body_pixel) {
                b = tunneler_map_get_block(&p_wz->map, new_x + sx, new_y + sy);
                if (block_is_breakable(b)) {
                    int bfr = warzone_set_block(p_wz, new_x + sx, new_y + sy, BLOCK_EMPTY);
                }
            }


        }
    }
}

void _engine_uwz_move_tank(engine *p, game_room *p_gr, warzone *p_wz, int i) {
    int dx, dy, sx, sy, new_x, new_y;
    enum direction new_direction;
    shape shape_belt, shape_body;
    block b;


    player *p_player = p_gr->players + i;
    tank *p_tank = p_wz->tanks + i;

    dx = controls_direction_x(&p_player->input);
    dy = controls_direction_y(&p_player->input);
    if (dx == 0 && dy == 0) {
        return;
    }

    new_direction = direction_get_by_d(dx, dy);
    shape_belt = shape_get(p_tank->direction, SHAPE_TANK_BELT);
    shape_body = shape_get(p_tank->direction, SHAPE_TANK_BODY);
    new_x = p_tank->location.x + dx;
    new_y = p_tank->location.y + dy;

    if (!_warzone_can_tank_move_to(p_wz, new_x, new_y, shape_belt, shape_body)) {
        return;
    }

    p_tank->direction = new_direction;
    p_tank->location.x = new_x;
    p_tank->location.y = new_y;

    _warzone_break_blocks_around(p_wz, new_x, new_y, shape_belt, shape_body);
}

void _engine_update_gameroom_battle(engine *p, game_room *p_gr) {
    int i;
    warzone *p_wz = &p_gr->zone;
    p_gr->current_tick++;
    if (p_gr->current_tick % 3 == 0) {
        for (i = 0; i < p_wz->tanks_size; i++) {
            if (p_wz->tanks[i].status != TANK_STATUS_OPERATIVE) {
                continue;
            }
            _engine_uwz_move_tank(p, p_gr, p_wz, i);
            engine_gameroom_process_map_changes(p, p_gr);
        }
    }

    if ((p_gr->current_tick % 9) == 0) {
        engine_gameroom_sync_tanks(p, p_gr);
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

void engine_gameroom_process_map_changes(engine *p, game_room *p_gr) {
    int i, n = 0;
    warzone *p_wz = &p_gr->zone;
    network_command_prepare(p->p_cmd_out, NCT_MAP_BLOCK_CHANGES);
    network_command_append_byte(p->p_cmd_out, 0);
    for (i = 0; i < p_wz->map_change_count; i++) {
        if (p->p_cmd_out->length + NETWORK_COMMAND_BLOCK_CHANGE_LENGTH >= NETWORK_COMMAND_DATA_LENGTH) {
            write_hex_byte(p->p_cmd_out->data, n);
            engine_bc_command(p, p_gr, p->p_cmd_out);
            network_command_prepare(p->p_cmd_out, NCT_MAP_BLOCK_CHANGES);
            n = 0;
        }
        network_command_append_short(p->p_cmd_out, p_wz->map_change_buffer[i].x);
        network_command_append_short(p->p_cmd_out, p_wz->map_change_buffer[i].y);
        network_command_append_char(p->p_cmd_out, char_from_num(p_wz->map_change_buffer[i].b));
        n++;
    }

    if (n > 0) {
        write_hex_byte(p->p_cmd_out->data, n);
        engine_bc_command(p, p_gr, p->p_cmd_out);
    }

    p_wz->map_change_count = 0;
}

void engine_gameroom_sync_tanks(engine *p, game_room *p_gr) {
    int i;
    tank *p_tank;
    for (i = 0; i < p_gr->size; i++) {
        p_tank = p_gr->zone.tanks + i;
        if (p_tank->status == TANK_STATUS_EMPTY) {
            continue;
        }
        engine_pack_game_tank(p->p_cmd_out, p_tank, i);
        engine_bc_command(p, p_gr, p->p_cmd_out);
    }
}
