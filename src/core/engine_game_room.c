#include "engine.h"

#include "../logger.h"
#include "../map_generator/map_generator.h"

void _engine_update_game_room(game_room *p_gr) {

}

void _engine_update_game_rooms(engine *p) {
    int i;
    for (i = 0; i < p->resources->game_rooms_length; i++) {
        _engine_update_game_room(p->resources->game_rooms + 1);
    }
}

void engine_game_room_cleanup(engine *p, game_room *p_gr) {
    int i;
    network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
    network_command_append_str(p->p_cmd_out, "Game room is being cleaned up");
    engine_bc_command(p, p_gr, p->p_cmd_out);

    game_room_clean_up(p_gr);
    glog(LOG_INFO, "Room %d has bean cleaned up", p_gr - p->resources->game_rooms);
}

void engine_game_room_client_disconnected(engine *p, net_client *p_cli, game_room *p_gr, char *reason) {
    int clientRID, i, new_leader_clientRID;

    clientRID = game_room_find_client(p_gr, p_cli);
    if (clientRID == ITEM_EMPTY) {
        return;
    }

    network_command_prepare(p->p_cmd_out, NCT_ROOM_CLIENT_REMOVE);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_str(p->p_cmd_out, reason);
    engine_bc_command(p, p_gr, p->p_cmd_out);

    if (p_gr->state == GAME_ROOM_STATE_LOBBY) {
        for (i = 0; i < p_gr->size; i++) {
            if (p_gr->players[i].client_rid != clientRID) {
                continue;
            }
            game_room_detach_player(p_gr, i);
            network_command_prepare(p->p_cmd_out, NCT_ROOM_PLAYER_DETACH);
            network_command_append_byte(p->p_cmd_out, i);
            engine_bc_command(p, p_gr, p->p_cmd_out);
        }
        game_room_remove_client(p_gr, p_cli);
    }

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

void _engine_clean_game_room(engine *p, game_room *p_gr) {

}

void _game_room_init_map(game_room *p_gr) {
    map_generator_generate(&p_gr->zone.map);
}

void engine_game_room_begin(engine *p, game_room *p_gr) {
    p_gr->state = GAME_ROOM_STATE_RUNNING;
    network_command_prepare(p->p_cmd_out, NCT_ROOM_SYNC_PHASE);
    
    _game_room_init_map(p_gr);

}