#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "engine.h"
#include "../data/resources.h"

#include "../networks/netadapter.h"

#include "../settings.h"
#include "summary.h"
#include "../game/control_input.h"

#define CLOCK CLOCK_MONOTONIC

int engine_init(engine *p_engine, settings *p_settings, resources *p_resources) {
    memset(p_engine, 0, sizeof (engine));
    p_engine->settings = p_settings;
    p_engine->resources = p_resources;
    p_engine->keep_running = 1;

    unsigned long milliseconds = 1000 / p_settings->MAX_TICKRATE;

    printf("Engine: Sleep millis = %lu\n", milliseconds);

    p_engine->sleep.tv_sec = milliseconds / 1000;
    p_engine->sleep.tv_nsec = (milliseconds % 1000) * 1000000;

    printf("Engine: Initialising summaries\n");
    summary_init(&p_engine->summary);

    return 0;
}

// fixme: engine composition

void engine_handle_command(netadapter *p, const network_command *cmd_in) {
    network_command cmd_out;
    netadapter *p_na = p;
    net_client *p_client = (p_na->clients + cmd_in->client_aid);

    switch (cmd_in->type) {
        default:
            cmd_out.type = NET_CMD_UNDEFINED;
            memcpy(cmd_out.data, "Cmd type unrecognised\0", 22);
            netadapter_send_command(p_client, &cmd_out);
            break;
        case NET_CMD_MSG_RCON:
            cmd_out.id = 66;
            memset(cmd_out.data, 0, NETWORK_COMMAND_DATA_LENGTH);
            strrev(cmd_out.data, cmd_in->data, cmd_in->length);

            netadapter_send_command(p_client, &cmd_out);
            break;
        case NET_CMD_MSG_PLAIN:
            cmd_out.type = NET_CMD_MSG_PLAIN;
            memcpy(cmd_out.data, cmd_in->data, cmd_in->length);
            cmd_out.data[cmd_in->length] = '\0';
            netadapter_broadcast_command(p_na->clients, p_na->clients_size, &cmd_out);
            break;
    }
}

void *engine_run(void *args) {
    engine *p_engine = (engine *) args;

    p_engine->summary.run_start = clock();
    printf("Engine: Starting\n");
    while (p_engine->keep_running) {
        p_engine->total_ticks++;
        if (p_engine->total_ticks > p_engine->settings->MAX_TICKRATE * 30) {

        }
        nanosleep(&p_engine->sleep, NULL);
    }

    p_engine->summary.run_end = clock();
    printf("Engine: Finished\n");
    netadapter_shutdown(&p_engine->netadapter);
    return NULL;
}
