#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "engine.h"
#include "resources.h"

#include "../networks/netadapter.h"

#include "../logger.h"
#include "../settings.h"
#include "../statistics.h"
#include "../game/control_input.h"

#define CLOCK CLOCK_MONOTONIC

int engine_init(engine *p_engine, settings *p_settings, resources *p_resources) {
    memset(p_engine, 0, sizeof (engine));
    p_engine->settings = p_settings;
    p_engine->resources = p_resources;
    p_engine->keep_running = 1;

    unsigned long milliseconds = 1000 / p_settings->MAX_TICKRATE;

    glog(LOG_FINE, "Engine: Sleep millis = %lu", milliseconds);

    p_engine->sleep.tv_sec = milliseconds / 1000;
    p_engine->sleep.tv_nsec = (milliseconds % 1000) * 1000000;

    glog(LOG_INFO, "Engine: Initialising summaries");
    statistics_init(&p_engine->stats);

    return 0;
}

void _engine_handle_command(void *handler, const network_command cmd) {
    engine *p_engine = (engine *) handler;
    netadapter *p_na = &p_engine->netadapter;
    net_client *p_client = netadapter_get_client_by_aid(p_na, cmd.remote_identifier);

    network_command cmd_out;
    memset(&cmd_out, 0, sizeof (network_command));

    switch (cmd.type) {
        default:
            cmd_out.type = NCT_UNDEFINED;
            memcpy(cmd_out.data, "Cmd type unrecognised", 22);
            netadapter_send_command(p_na, &p_client->connection, &cmd_out);
            break;
        case NCT_ROOM_CLIENT_INTRODUCE:
            net_client_set_name(p_client, cmd.data, cmd._length);
            cmd_out.type = NCT_LEAD_APPROVE;
            netadapter_send_command(p_na, &p_client->connection, &cmd_out);
            break;
        case NCT_MSG_RCON:
            memset(cmd_out.data, 0, NETWORK_COMMAND_DATA_LENGTH);
            strrev(cmd_out.data, cmd.data, cmd._length);

            netadapter_send_command(p_na, &p_client->connection, &cmd_out);
            break;
        case NCT_MSG_PLAIN:
            cmd_out.type = NCT_MSG_PLAIN;
            memcpy(cmd_out.data, cmd.data, cmd._length);
            cmd_out.data[cmd._length] = '\0';
            netadapter_broadcast_command(p_na, p_na->clients, p_na->clients_length, &cmd_out);
            break;
    }
}

int _engine_link_netadapter(engine *p) {
    p->netadapter.command_handler = p;
    p->netadapter.command_handle_func = &_engine_handle_command;
}

void *engine_run(void *args) {
    engine *p_engine = (engine *) args;

    _engine_link_netadapter(p_engine);

    p_engine->stats.run_start = clock();
    glog(LOG_INFO, "Engine: Starting");
    while (p_engine->keep_running) {
        p_engine->total_ticks++;
        if (p_engine->total_ticks > p_engine->settings->MAX_TICKRATE * 30) {

        }
        nanosleep(&p_engine->sleep, NULL);
    }

    p_engine->stats.run_end = clock();
    glog(LOG_INFO, "Engine: Finished");
    netadapter_shutdown(&p_engine->netadapter);
    return NULL;
}

int engine_count_temp_connections(engine *p, unsigned char status) {
    int i, n = 0;
    for (i = 0; i < p->resources->connectons_length; i++) {
        if (status == TCP_CONNECTION_STATUS_ANY || (p->resources->connections + i)->status == status) {
            n++;
        }
    }
    return n;
}

int engine_count_clients(engine *p, unsigned char status) {
    int i, n = 0;
    for (i = 0; i < p->resources->clients_length; i++) {
        if (status == TCP_CONNECTION_STATUS_ANY || (p->resources->clients + i)->connection.status == status) {
            n++;
        }
    }
    
    return n;
}
