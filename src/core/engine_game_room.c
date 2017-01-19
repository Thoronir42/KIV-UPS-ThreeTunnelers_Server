#include "engine.h"

#include "../logger.h"
#include "../map_generator/map_generator.h"

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

tunneler_map *_game_room_init_map(game_room *p_gr, int player_count) {
    int i, n;
    tunneler_map *p_map = &p_gr->zone.map;

    map_generator_generate(p_map, player_count);

    for (i = 0; i < p_gr->size; i++) {
        if (p_gr->players[i].client_rid == ITEM_EMPTY) {
            continue;
        }
        tunneler_map_assign_base(p_map, n++, i);
    }

    return p_map;

}

void engine_game_room_set_state(engine *p, game_room *p_gr, game_room_state game_state){
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

    network_command_prepare(p->p_cmd_out, NCT_MAP_SPECIFICATION);
    network_command_append_byte(p->p_cmd_out, p_map->CHUNK_SIZE);
    network_command_append_byte(p->p_cmd_out, p_map->chunk_dimensions.width);
    network_command_append_byte(p->p_cmd_out, p_map->chunk_dimensions.height);

    engine_bc_command(p, p_gr, p->p_cmd_out);
    
    engine_pack_map_bases(p->p_cmd_out, p_map);
    engine_bc_command(p, p_gr, p->p_cmd_out);

    for (y = 0; y < p_map->chunk_dimensions.height; y++) {
        for (x = 0; x < p_map->chunk_dimensions.width; x++) {
            engine_pack_chunk(p->p_cmd_out, tunneler_map_get_chunk(p_map, x, y));
            engine_bc_command(p, p_gr, p->p_cmd_out);   
        }
    }
}

void engine_pack_map_bases(network_command *p_dst, tunneler_map *p_map){
    int i, x, y;
    tunneler_map_chunk *p_chunk;
    
    network_command_prepare(p_dst, NCT_MAP_BASES);
    network_command_append_byte(p_dst, p_map->bases_size);
    
    for(i = 0; i < p_map->bases_size; i++){
        network_command_append_byte(p_dst, x = p_map->bases[i].x);
        network_command_append_byte(p_dst, y = p_map->bases[i].y);
        
        p_chunk = tunneler_map_get_chunk(p_map, x, y);
        
        network_command_append_byte(p_dst, p_chunk->assigned_player_rid);
    }
}

void engine_pack_chunk(network_command *p_dst, tunneler_map_chunk *p_chunk) {
    int x, y;
    block b;

    network_command_prepare(p_dst, NCT_MAP_CHUNK_DATA);

    for (y = 0; y < p_chunk->size; y++) {
        for (x = 0; x < p_chunk->size; x++) {
            b = tunneler_map_chunk_get_block(p_chunk, x, y);
            network_command_append_char(p_dst, b);
        }
    }

}
