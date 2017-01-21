#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "engine.h"
#include "resources.h"

#include "../networks/netadapter.h"

#include "../logger.h"
#include "../settings.h"
#include "../statistics.h"
#include "../game/control_input.h"
#include "../structures/cmd_queue.h"
#include "../localisation.h"

#define CLOCK CLOCK_MONOTONIC

int engine_init(engine *p_engine, settings *p_settings, resources *p_resources) {
    memset(p_engine, 0, sizeof (engine));

    p_engine->settings = p_settings;
    p_engine->resources = p_resources;
    p_engine->keep_running = 1;

    statistics_init(&p_engine->stats);
    cmd_queue_init(&p_engine->cmd_in_queue);

    unsigned long milliseconds = 1000000000 / p_settings->MAX_TICKRATE;

    p_engine->sleep.tv_sec = milliseconds / 1000000000;
    p_engine->sleep.tv_nsec = (milliseconds % 1000000000);

    glog(LOG_FINE, "Engine: Sleep = %d s + %lu ns", p_engine->sleep.tv_sec, p_engine->sleep.tv_nsec);

    _engine_init_solo_commands(p_engine->command_proccess_func);
    _engine_init_gameroom_commands(p_engine->command_proccess_func);
    _engine_init_game_play_commands(p_engine->command_proccess_func);

    p_engine->p_netadapter = &p_engine->netadapter;
    p_engine->p_cmd_out = &p_engine->_cmd_out;

    return 0;
}

void _engine_handle_command(void *handler, const network_command cmd) {
    engine *p_engine = (engine *) handler;

    if (cmd_queue_is_full(&p_engine->cmd_in_queue)) {
        glog(LOG_ERROR, "Command queue is full");
        return;
    }

    cmd_queue_put(&p_engine->cmd_in_queue, cmd);
}

int engine_count_clients(engine *p, net_client_status status) {
    int i, n = 0;
    net_client *p_client;
    for (i = 0; i < p->resources->clients_length; i++) {
        p_client = p->resources->clients + i;
        if (status == NET_CLIENT_STATUS_ANY ||
                (p_client->connection == NULL && (status == NET_CLIENT_STATUS_EMPTY)) ||
                (p_client->connection != NULL && (status == p_client->status))
                ) {
            n++;
        }
    }

    return n;
}

net_client *engine_first_free_client_offset(engine *p) {
    int i;
    net_client *p_cli;
    for (i = 0; i < p->resources->clients_length; i++) {
        p_cli = p->resources->clients + i;
        if (p_cli->status == NET_CLIENT_STATUS_EMPTY) {
            return p_cli;
        }
    }

    return NULL;
}

net_client *engine_client_by_socket(engine *p, int socket) {
    if (socket < 0 || socket > p->resources->connections_length) {
        return NULL;
    }
    if (p->resources->con_to_cli[socket] == ITEM_EMPTY) {
        return NULL;
    }
    return p->resources->clients + p->resources->con_to_cli[socket];
}

net_client *engine_client_by_secret(engine *p, char *secret) {
    int i;
    net_client *p_cli;
    for (i = 0; i < p->resources->clients_length; i++) {
        p_cli = p->resources->clients + i;
        if (!strcmp(p_cli->connection_secret, secret)) {
            return p_cli;
        }
    }

    return NULL;
}

// netowrk command action related stuff

void engine_send_command(engine *p, net_client *p_cli, network_command *cmd) {
    netadapter_send_command(&p->netadapter, p_cli->connection, cmd);
}

void engine_bc_command(engine *p, game_room *p_gr, network_command *cmd) {
    netadapter_broadcast_command_p(&p->netadapter, p_gr->clients, p_gr->size, cmd);
}

game_room *engine_game_room_by_id(engine *p, int room_id) {
    if (room_id < 0 || room_id > p->resources->game_rooms_length) {
        return NULL;
    }

    return p->resources->game_rooms + room_id;
}

game_room *engine_find_empty_game_room(engine *p) {
    int i;
    game_room *p_gr;

    for (i = 0; i < p->resources->game_rooms_length; i++) {
        p_gr = p->resources->game_rooms + i;
        if (p_gr->state == GAME_ROOM_STATE_IDLE) {
            return p_gr;
        }
    }

    return NULL;
}

void _engine_dump_room_to_client(engine *p, net_client *p_cli, game_room *p_gr) {
    glog(LOG_WARNING, "Engine: Room dumping not implemented yet");
}

void engine_announce_client_left(engine *p, game_room *p_gr, int clientRID, char *reason) {
    network_command_prepare(p->p_cmd_out, NCT_ROOM_CLIENT_REMOVE);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_str(p->p_cmd_out, reason);

    engine_bc_command(p, p_gr, p->p_cmd_out);
}

void engine_client_disconnected(engine *p, net_client *p_cli, char *reason) {
    game_room *p_gr;
    p_gr = engine_game_room_by_id(p, p_cli->room_id);

    if (p_gr != NULL && p_gr->state == GAME_ROOM_STATE_LOBBY) {
        engine_game_room_client_disconnected(p, p_gr, p_cli, reason);
        p_cli->status = NET_CLIENT_STATUS_DISCONNECTED;

        if (p_gr->state == GAME_ROOM_STATE_IDLE) {
            net_client_wipe(p_cli);
        }
    } else {
        net_client_wipe(p_cli);
    }
}

void engine_pack_map_bases(network_command *p_dst, tunneler_map *p_map) {
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

void engine_pack_chunk(network_command *p_dst, int x, int y, tunneler_map_chunk *p_chunk) {
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