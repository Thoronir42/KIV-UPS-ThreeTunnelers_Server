#include "engine.h"

#include "../logger.h"

#include "../my_strings.h"
#include "../map_generator/map_generator.h"
#include "../networks/network_command_factory.h"

void engine_game_room_cleanup(engine *p, game_room *p_gr) {
    int i;
    network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
    network_command_append_str(p->p_cmd_out, "Game room is being cleaned up");
    engine_bc_command(p, p_gr, p->p_cmd_out);

    game_room_clean_up(p_gr);

    for (i = 0; i < GAME_ROOM_MAX_PLAYERS; i++) {
        if (p_gr->clients[i] == NULL) {
            continue;
        }
        net_client_wipe(p_gr->clients[i]);
    }

    glog(LOG_INFO, "Room %d has bean cleaned up", p_gr - p->resources->game_rooms);
}

void engine_game_room_remove_client(engine *p, game_room *p_gr, net_client *p_cli) {
    int i, clientRID = game_room_find_client(p_gr, p_cli);

    if (clientRID == ITEM_EMPTY) {
        return;
    }

    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->players[i].client_rid != clientRID) {
            continue;
        }
        engine_game_room_remove_player(p, p_gr, i);
    }

    p_gr->clients[clientRID] = NULL;
}

void engine_game_room_put_client(engine *p, game_room *p_gr, net_client *p_cli) {
    int client_rid, playerRID;
    int room_id;

    room_id = p_gr - p->resources->game_rooms;

    if (!game_room_has_open_slots(p_gr)) {
        network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
        network_command_append_str(p->p_cmd_out, "Game room is full");
        engine_send_command(p, p_cli, p->p_cmd_out);
        return;
    }
    if (p_gr->state != GAME_ROOM_STATE_LOBBY) {
        network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
        network_command_append_str(p->p_cmd_out, "Game room is not in lobby");
        engine_send_command(p, p_cli, p->p_cmd_out);
        return;
    }

    client_rid = game_room_put_client(p_gr, p_cli);

    nc_create_rooms_join(p->p_cmd_out, room_id, client_rid, p_gr->leaderClientRID);
    engine_send_command(p, p_cli, p->p_cmd_out);
    p_cli->room_id = room_id;

    engine_game_room_dump_to_client(p, p_gr, p_cli);

    nc_create_room_client_introduce(p->p_cmd_out, p_gr, client_rid);
    engine_bc_command(p, p_gr, p->p_cmd_out);

    engine_game_room_put_player(p, p_gr, p_cli);
}

void engine_game_room_put_client_again(engine *p, game_room *p_gr, net_client *p_cli) {
    int client_rid, playerRID;
    int room_id;

    room_id = p_gr - p->resources->game_rooms;
    client_rid = game_room_find_client(p_gr, p_cli);

    glog(LOG_INFO, "Room %d: Client %d reconnected, getting them in loop "
            "with current state", room_id, client_rid);
    nc_create_rooms_join(p->p_cmd_out, room_id, client_rid, p_gr->leaderClientRID);
    engine_send_command(p, p_cli, p->p_cmd_out);

    engine_game_room_dump_to_client(p, p_gr, p_cli);
    engine_game_room_dump_map_to_client(p, p_cli, &p_gr->zone.map);

    network_command_prepare(p->p_cmd_out, NCT_ROOM_SYNC_STATE);
    network_command_append_byte(p->p_cmd_out, GAME_ROOM_STATE_BATTLE);
    engine_send_command(p, p_cli, p->p_cmd_out);

    p_cli->status = NET_CLIENT_STATUS_PLAYING;

    nc_create_client_status(p->p_cmd_out, client_rid, p_cli->status, 0);
    engine_bc_command(p, p_gr, p->p_cmd_out);
}

void engine_game_room_client_disconnected(engine *p, game_room *p_gr, net_client *p_cli, char *reason) {
    int clientRID, i, new_leader_client_rid;

    clientRID = game_room_find_client(p_gr, p_cli);
    if (clientRID == ITEM_EMPTY) {
        return;
    }

    if (p_gr->state == GAME_ROOM_STATE_LOBBY) {
        engine_game_room_remove_client(p, p_gr, p_cli);

        network_command_prepare(p->p_cmd_out, NCT_ROOM_CLIENT_REMOVE);
        network_command_append_byte(p->p_cmd_out, clientRID);
        network_command_append_str(p->p_cmd_out, reason);
    } else {
        nc_create_client_status(p->p_cmd_out, clientRID, NET_CLIENT_STATUS_DISCONNECTED, 0);
    }
    engine_bc_command(p, p_gr, p->p_cmd_out);

    if (p_gr->leaderClientRID == clientRID) {
        new_leader_client_rid = game_room_choose_leader_other_than(p_gr, p_cli);
        if (new_leader_client_rid == ITEM_EMPTY) {
            glog(LOG_INFO, "Leader of room %d left. No other client is equilibe"
                    " to be leader, cleaning room up.",
                    p_gr - p->resources->game_rooms);
            engine_game_room_cleanup(p, p_gr);
            return;
        }
        glog(LOG_INFO, "Leader of room %d left. New leader is %d",
                p_gr - p->resources->game_rooms, new_leader_client_rid);
        network_command_prepare(p->p_cmd_out, NCT_ROOM_SET_LEADER);
        network_command_append_byte(p->p_cmd_out, new_leader_client_rid);
        engine_bc_command(p, p_gr, p->p_cmd_out);
    }
}

int engine_game_room_put_player(engine *p, game_room *p_gr, net_client *p_cli) {
    int player_rid, client_rid, player_cid;
    client_rid = game_room_find_client(p_gr, p_cli);

    player_rid = game_room_attach_player(p_gr, client_rid);
    if (player_rid == ITEM_EMPTY) {
        glog(LOG_WARNING, "No empty spot to put player of client %d", client_rid);
        return 1;
    }
    player_cid = net_client_put_player(p_cli, player_rid);
    if (player_cid == ITEM_EMPTY) {
        game_room_detach_player(p_gr, player_rid);
        glog(LOG_WARNING, "Failed to put player to client");
        return 1;
    }

    glog(LOG_INFO, "Room %d: Client %d added player %d as their player %d",
            p_gr - p->resources->game_rooms, client_rid, player_rid, player_cid);
    nc_create_room_player(p->p_cmd_out, p_gr, player_rid);
    engine_bc_command(p, p_gr, p->p_cmd_out);


    return 0;
}

void engine_game_room_remove_player(engine *p, game_room *p_gr, int playerRID) {
    // todo: client cleanup
    game_room_detach_player(p_gr, playerRID);
    network_command_prepare(p->p_cmd_out, NCT_ROOM_PLAYER_DETACH);
    network_command_append_byte(p->p_cmd_out, playerRID);
    engine_bc_command(p, p_gr, p->p_cmd_out);
}

tunneler_map *_game_room_init_map(game_room *p_gr, int player_count) {
    int i, base_index;
    intpoint base_center;
    tunneler_map *p_map = &p_gr->zone.map;
    tunneler_map_init(p_map, 8, 6, 21);

    map_generator_generate(p_map, player_count);

    base_index = 0;
    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->players[i].client_rid == ITEM_EMPTY) {
            continue;
        }
        base_center = tunneler_map_assign_base(p_map, base_index++, i);
        warzone_init_tank(&p_gr->zone, i, base_center);
    }

    return p_map;

}

void _engine_game_room_set_clients_status(game_room *p_gr, net_client_status status) {
    int i;
    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->clients[i] == NULL) {
            continue;
        }
        switch (p_gr->clients[i]->status) {
            case NET_CLIENT_STATUS_DISCONNECTED:
            case NET_CLIENT_STATUS_EMPTY:
                continue;
        }

        p_gr->clients[i]->status = status;
    }
}

void _engine_game_room_clear_controls(game_room *p_gr) {
    int i;
    for (i = 0; i < p_gr->size; i++) {
        controls_set_state(&p_gr->players[i].input, 0);
    }
}

void engine_game_room_set_state(engine *p, game_room *p_gr, game_room_state game_state) {
    int i;
    p_gr->state = game_state;
    switch (p_gr->state = game_state) {
        case GAME_ROOM_STATE_BATTLE:
            _engine_game_room_set_clients_status(p_gr, NET_CLIENT_STATUS_PLAYING);
            _engine_game_room_clear_controls(p_gr);
            p_gr->current_tick = 0;
            break;
        case GAME_ROOM_STATE_LOBBY:
            _engine_game_room_set_clients_status(p_gr, NET_CLIENT_STATUS_CONNECTED);
            break;
    }

    network_command_prepare(p->p_cmd_out, NCT_ROOM_SYNC_STATE);
    network_command_append_byte(p->p_cmd_out, game_state);

    engine_bc_command(p, p_gr, p->p_cmd_out);

    for (i = 0; i < p_gr->size; i++) {
        p_gr->ready_state[i] = 0;
    }
}

void engine_game_room_begin(engine *p, game_room *p_gr) {
    tunneler_map *p_map;
    int x, y, player_count = game_room_count_players(p_gr);
    warzone_init(&p_gr->zone, player_count);
    p_map = _game_room_init_map(p_gr, player_count);

    glog(LOG_INFO, "Created map %d chunks wide, %d chunks high, each chunk "
            "having %d^2 blocks", p_map->chunk_dimensions.width,
            p_map->chunk_dimensions.width, p_map->CHUNK_SIZE);

    nc_create_map_specification(p->p_cmd_out, p_map);
    engine_bc_command(p, p_gr, p->p_cmd_out);

    nc_create_map_bases(p->p_cmd_out, p_map);
    engine_bc_command(p, p_gr, p->p_cmd_out);

    for (y = 0; y < p_map->chunk_dimensions.height; y++) {
        for (x = 0; x < p_map->chunk_dimensions.width; x++) {
            nc_create_map_chunk(p->p_cmd_out, x, y, tunneler_map_get_chunk(p_map, x, y));
            engine_bc_command(p, p_gr, p->p_cmd_out);
        }
    }
}

void engine_game_room_dump_to_client(engine *p, game_room *p_gr, net_client *p_cli) {
    int i;
    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->clients[i] == NULL) {
            continue;
        }
        nc_create_room_client_introduce(p->p_cmd_out, p_gr, i);
        engine_send_command(p, p_cli, p->p_cmd_out);

        network_command_prepare(p->p_cmd_out, NCT_ROOM_READY_STATE);
        network_command_append_byte(p->p_cmd_out, p_gr->ready_state[i]);
        network_command_append_byte(p->p_cmd_out, i);
        engine_send_command(p, p_cli, p->p_cmd_out);
    }
    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->players[i].client_rid == ITEM_EMPTY) {
            continue;
        }
        nc_create_room_player(p->p_cmd_out, p_gr, i);
        engine_send_command(p, p_cli, p->p_cmd_out);
    }

    if (p_gr->state != GAME_ROOM_STATE_BATTLE) {
        return;
    }
}

void engine_game_room_dump_map_to_client(engine *p, net_client *p_cli, tunneler_map *p_map) {
    int x, y;

    nc_create_map_specification(p->p_cmd_out, p_map);
    engine_send_command(p, p_cli, p->p_cmd_out);

    nc_create_map_bases(p->p_cmd_out, p_map);
    engine_send_command(p, p_cli, p->p_cmd_out);

    for (y = 0; y < p_map->chunk_dimensions.height; y++) {
        for (x = 0; x < p_map->chunk_dimensions.width; x++) {
            nc_create_map_chunk(p->p_cmd_out, x, y, tunneler_map_get_chunk(p_map, x, y));
            engine_send_command(p, p_cli, p->p_cmd_out);
        }
    }
}
