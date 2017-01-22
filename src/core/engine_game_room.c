#include "engine.h"

#include "../logger.h"

#include "../my_strings.h"
#include "../map_generator/map_generator.h"

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
    int clientRID, playerRID;
    int room_id;

    room_id = p_gr - p->resources->game_rooms;

    if (!game_room_is_joinable(p_gr)) {
        network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
        network_command_append_str(p->p_cmd_out, "Game room is full");
        engine_send_command(p, p_cli, p->p_cmd_out);
        return;
    }
    clientRID = game_room_put_client(p_gr, p_cli);

    network_command_prepare(p->p_cmd_out, NCT_ROOMS_JOIN);
    network_command_append_byte(p->p_cmd_out, room_id);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_byte(p->p_cmd_out, p_gr->leaderClientRID);

    p_cli->room_id = room_id;
    engine_send_command(p, p_cli, p->p_cmd_out);
    engine_game_room_dump_to_client(p, p_cli, p_gr);

    network_command_prepare(p->p_cmd_out, NCT_ROOM_CLIENT_INFO);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_str(p->p_cmd_out, p_cli->name);

    engine_bc_command(p, p_gr, p->p_cmd_out);

    engine_game_room_put_player(p, p_gr, clientRID);

    return;
}

void engine_game_room_client_disconnected(engine *p, game_room *p_gr, net_client *p_cli, char *reason) {
    int clientRID, i, new_leader_clientRID;

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
        network_command_prepare(p->p_cmd_out, NCT_ROOM_CLIENT_STATUS);
        network_command_append_byte(p->p_cmd_out, clientRID);
        network_command_append_byte(p->p_cmd_out, NET_CLIENT_STATUS_DISCONNECTED);
        network_command_append_short(p->p_cmd_out, 0);
    }
    engine_bc_command(p, p_gr, p->p_cmd_out);

    if (p_gr->leaderClientRID == clientRID) {
        new_leader_clientRID = game_room_choose_leader_other_than(p_gr, p_cli);
        glog(LOG_INFO, "Leader of room %d left. New leader is %d",
                p_gr - p->resources->game_rooms, new_leader_clientRID);
        if (new_leader_clientRID == ITEM_EMPTY) {
            engine_game_room_cleanup(p, p_gr);
            return;
        }
        network_command_prepare(p->p_cmd_out, NCT_ROOM_SET_LEADER);
        network_command_append_byte(p->p_cmd_out, new_leader_clientRID);
        engine_bc_command(p, p_gr, p->p_cmd_out);
    }
}

int engine_game_room_put_player(engine *p, game_room *p_gr, int clientRID) {
    int playerRID;
    playerRID = game_room_attach_player(p_gr, clientRID);
    if (playerRID == ITEM_EMPTY) {
        return 1;
    }

    network_command_prepare(p->p_cmd_out, NCT_ROOM_PLAYER_ATTACH);
    network_command_append_byte(p->p_cmd_out, playerRID);
    network_command_append_byte(p->p_cmd_out, p_gr->players[playerRID].color);
    network_command_append_byte(p->p_cmd_out, clientRID);
    engine_bc_command(p, p_gr, p->p_cmd_out);


    return 0;
}

void engine_game_room_remove_player(engine *p, game_room *p_gr, int playerRID) {
    game_room_detach_player(p_gr, playerRID);
    network_command_prepare(p->p_cmd_out, NCT_ROOM_PLAYER_DETACH);
    network_command_append_byte(p->p_cmd_out, playerRID);
    engine_bc_command(p, p_gr, p->p_cmd_out);
}

tunneler_map *_game_room_init_map(game_room *p_gr, int player_count) {
    int i, n;
    tunneler_map *p_map = &p_gr->zone.map;
    tunneler_map_init(p_map, 8, 6, 21);

    map_generator_generate(p_map, player_count);

    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->players[i].client_rid == ITEM_EMPTY) {
            continue;
        }
        tunneler_map_assign_base(p_map, n++, i);
    }

    return p_map;

}

void engine_game_room_set_state(engine *p, game_room *p_gr, game_room_state game_state) {
    p_gr->state = game_state;
    network_command_prepare(p->p_cmd_out, NCT_ROOM_SYNC_STATE);
    network_command_append_byte(p->p_cmd_out, game_state);

    engine_bc_command(p, p_gr, p->p_cmd_out);

}

void engine_game_room_begin(engine *p, game_room *p_gr) {
    tunneler_map *p_map;
    int x, y, player_count = game_room_count_players(p_gr);
    warzone_init(&p_gr->zone, player_count);
    p_map = _game_room_init_map(p_gr, player_count);

    glog(LOG_INFO, "Created map %d chunks wide, %d chunks high, each chunk "
            "having %d^2 blocks", p_map->chunk_dimensions.width,
            p_map->chunk_dimensions.width, p_map->CHUNK_SIZE);

    engine_pack_map_specification(p->p_cmd_out, p_map);
    engine_bc_command(p, p_gr, p->p_cmd_out);

    engine_pack_map_bases(p->p_cmd_out, p_map);
    engine_bc_command(p, p_gr, p->p_cmd_out);

    for (y = 0; y < p_map->chunk_dimensions.height; y++) {
        for (x = 0; x < p_map->chunk_dimensions.width; x++) {
            engine_pack_map_chunk(p->p_cmd_out, x, y, tunneler_map_get_chunk(p_map, x, y));
            engine_bc_command(p, p_gr, p->p_cmd_out);
        }
    }
}

void engine_game_room_dump_to_client(engine *p, net_client *p_cli, game_room *p_gr) {
    glog(LOG_WARNING, "Engine: Room dumping not implemented yet");
}
