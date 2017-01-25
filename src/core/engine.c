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
#include "../game/controls.h"
#include "../structures/cmd_queue.h"
#include "../localisation.h"

#define CLOCK CLOCK_MONOTONIC

int engine_init(engine *p, settings *p_settings, resources *p_resources) {
    memset(p, 0, sizeof (engine));

    p->settings = p_settings;
    p->resources = p_resources;
    p->keep_running = 1;

    statistics_init(&p->stats);
    cmd_queue_init(&p->cmd_in_queue);

    unsigned long milliseconds = 1000000000 / p_settings->MAX_TICKRATE;

    p->sleep.tv_sec = milliseconds / 1000000000;
    p->sleep.tv_nsec = (milliseconds % 1000000000);

    glog(LOG_FINE, "Engine: Sleep = %d s + %lu ns", p->sleep.tv_sec, p->sleep.tv_nsec);

    _engine_init_solo_commands(p->command_proccess_func);
    _engine_init_gameroom_commands(p->command_proccess_func);
    _engine_init_game_play_commands(p->command_proccess_func);

    _engine_init_gameroom_updates(p);

    p->p_netadapter = &p->netadapter;
    p->p_cmd_out = &p->_cmd_out;

    return 0;
}

void engine_handle_command(void *handler, const network_command cmd) {
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

void engine_bc_command_status_filter(engine *p, game_room *p_gr,
        network_command *cmd, net_client_status status) {
    netadapter_broadcast_command_pf(&p->netadapter, p_gr->clients, p_gr->size, cmd, status);
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

void engine_client_disconnected(engine *p, net_client *p_cli, char *reason) {
    game_room *p_gr;
    int client_id = p_cli - p->resources->clients;
    p_gr = engine_game_room_by_id(p, p_cli->room_id);

    if (p_gr == NULL) {
        glog(LOG_INFO, "Client %d was not in a room - wiping them immediately. (%s)", client_id, reason);
        net_client_wipe(p_cli);
        return;
    }

    engine_game_room_client_disconnected(p, p_gr, p_cli, reason);
    if (p_gr->state != GAME_ROOM_STATE_IDLE) {
        glog(LOG_INFO, "Cliend %d but is not being wiped yet - they might return. (%s)", client_id, reason);
        p_cli->status = NET_CLIENT_STATUS_DISCONNECTED;
    } else {
        glog(LOG_INFO, "Cliend %d left room which was cleaned up, cleaning client as well. (%s)", client_id, reason);
        net_client_wipe(p_cli);
    }
}

