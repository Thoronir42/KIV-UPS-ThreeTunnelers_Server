
#include "network_command.h"
#include "network_command_factory.h"
#include "net_client.h"

void nc_create_rooms_join(network_command *p, int room_id, int local_client_rid, int leader_client_rid){
    network_command_prepare(p, NCT_ROOMS_JOIN);
    network_command_append_byte(p, room_id);
    network_command_append_byte(p, local_client_rid);
    network_command_append_byte(p, leader_client_rid);
}

void nc_create_client_status(network_command *p, int client_rid, net_client_status status, int latency) {
    network_command_prepare(p, NCT_ROOM_CLIENT_STATUS);
    network_command_append_byte(p, client_rid);
    network_command_append_byte(p, status);
    network_command_append_short(p, latency);
}

void nc_create_room_client_introduce(network_command *p_dst, game_room *p_gr, int client_rid) {
    net_client *p_cli = p_gr->clients[client_rid];

    network_command_prepare(p_dst, NCT_ROOM_CLIENT_INTRODUCE);
    network_command_append_byte(p_dst, client_rid);
    network_command_append_str(p_dst, p_cli->name);
}

void nc_create_room_player(network_command *p_dst, game_room *p_gr, int player_rid) {
    player *p_plr = p_gr->players + player_rid;
    net_client *p_cli = p_gr->clients[p_plr->client_rid];
    int player_cid = net_client_player_cid_by_rid(p_cli, player_rid);

    network_command_prepare(p_dst, NCT_ROOM_PLAYER_ATTACH);
    network_command_append_byte(p_dst, player_rid);
    network_command_append_byte(p_dst, p_gr->players[player_rid].color);
    network_command_append_byte(p_dst, p_plr->client_rid);
    network_command_append_byte(p_dst, player_cid);
}

void nc_create_map_specification(network_command *p_dst, tunneler_map *p_map) {
    network_command_prepare(p_dst, NCT_MAP_SPECIFICATION);
    network_command_append_byte(p_dst, p_map->CHUNK_SIZE);
    network_command_append_byte(p_dst, p_map->chunk_dimensions.width);
    network_command_append_byte(p_dst, p_map->chunk_dimensions.height);
}

void nc_create_map_bases(network_command *p_dst, tunneler_map *p_map) {
    int i, x, y;
    tunneler_map_chunk *p_chunk;

    network_command_prepare(p_dst, NCT_MAP_BASES);
    network_command_append_byte(p_dst, p_map->bases_size);

    for (i = 0; i < p_map->bases_size; i++) {
        network_command_append_byte(p_dst, x = p_map->bases[i].x);
        network_command_append_byte(p_dst, y = p_map->bases[i].y);

        p_chunk = tunneler_map_get_chunk(p_map, x, y);

        network_command_append_byte(p_dst, p_chunk->assigned_player_rid);
    }
}

void nc_create_map_chunk(network_command *p_dst, int x, int y, tunneler_map_chunk *p_chunk) {
    int block_x, block_y;
    my_byte check_sum = 0;
    block b;


    network_command_prepare(p_dst, NCT_MAP_CHUNK_DATA);
    network_command_append_byte(p_dst, x);
    network_command_append_byte(p_dst, y);
    network_command_append_byte(p_dst, 0);

    for (block_y = 0; block_y < p_chunk->size; block_y++) {
        for (block_x = 0; block_x < p_chunk->size; block_x++) {
            b = tunneler_map_chunk_get_block(p_chunk, block_x, block_y);
            network_command_append_char(p_dst, char_from_num(b));
            check_sum = (check_sum + b) % 256;
        }
    }

    write_hex_byte(p_dst->data + 4, check_sum);
}

void nc_create_game_tank(network_command *p_dst, tank *p_tank, int player_rid) {
    network_command_prepare(p_dst, NCT_GAME_TANK_INFO);
    network_command_append_byte(p_dst, player_rid);
    network_command_append_byte(p_dst, p_tank->status);
    network_command_append_short(p_dst, p_tank->location.x);
    network_command_append_short(p_dst, p_tank->location.y);
    network_command_append_byte(p_dst, p_tank->direction);
    network_command_append_byte(p_dst, p_tank->hitpoints);
    network_command_append_byte(p_dst, p_tank->energy);
}