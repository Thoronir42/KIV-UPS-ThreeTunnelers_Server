#ifndef NETWORK_COMMAND_FACTORY_H
#define NETWORK_COMMAND_FACTORY_H

#include "network_command.h"

#include "net_client.h"
#include "../game/game_room.h"
#include "../model/tank.h"


void nc_create_rooms_join(network_command *p, int room_id, int local_client_rid, int leader_client_rid);

void nc_create_client_status(network_command *p, int client_rid, net_client_status status, int latency);

void nc_create_room_client_introduce(network_command *p_dst, game_room *p_gr, int client_rid);
void nc_create_room_player(network_command *p_dst, game_room *p_gr, int player_rid);

void nc_create_map_specification(network_command *p_dst, tunneler_map *p_map);
void nc_create_map_bases(network_command *p_dst, tunneler_map *p_map);
void nc_create_map_chunk(network_command *p_dst, int x, int y, tunneler_map_chunk *p_chunk);

void nc_create_game_tank(network_command *p_dst, tank *p_tank, int player_rid);

#endif /* NETWORK_COMMAND_FACTORY_H */

