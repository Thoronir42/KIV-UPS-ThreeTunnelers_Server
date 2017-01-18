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
    _engine_init_game_prep_commands(p_engine->command_proccess_func);
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

int engine_count_clients(engine *p, unsigned char status) {
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

void engine_put_client_into_room(engine *p, net_client *p_cli, game_room *p_gr) {
    int i, clientRID;
    int room_id;
    
    room_id = p_gr - p->resources->game_rooms;
    
    clientRID = game_room_put_client(p_gr, p_cli);
    if (clientRID == ITEM_EMPTY) {
        network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
        network_command_append_str(p->p_cmd_out, "Game room is full");
        engine_send_command(p, p_cli, p->p_cmd_out);
        
        return;
    }
    
    p_cli->room_id = room_id;

    network_command_prepare(p->p_cmd_out, NCT_ROOMS_JOIN);
    network_command_append_byte(p->p_cmd_out, room_id);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_byte(p->p_cmd_out, p_gr->leaderClient);

    engine_send_command(p, p_cli, p->p_cmd_out);
    
    network_command_prepare(p->p_cmd_out, NCT_ROOM_CLIENT_INFO);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_str(p->p_cmd_out, p_cli->name);

    engine_bc_command(p, p_gr, p->p_cmd_out);

    _engine_dump_room_to_client(p, p_cli, p_gr);

    return;
}


