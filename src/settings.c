#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


#include "settings.h"

// defaults
#define _CHUNK_SIZE 20
#define _MAX_TICKRATE 32
#define _MAX_PLAYERS_PER_ROOM 4

// mind the 0th argument which is program path
#define ARG_REQ_PORT 1
#define ARG_MAX_ROOMS 2

#define ARGERR_NOT_ENOUGH_ARGUMENTS -1

int settings_process_arguments(settings *p_settings, int argc, char *argv[]) {
    if (argc < 2) {
        return ARGERR_NOT_ENOUGH_ARGUMENTS;
    }

    p_settings->show_summaries = 1;

    int req_port = atoi(argv[ARG_MAX_ROOMS]);
    p_settings->port = req_port;

    int max_rooms = atoi(argv[ARG_MAX_ROOMS]);
    *(int *) &p_settings->MAX_ROOMS = max_rooms;
    
    *(int *) (&p_settings->CHUNK_SIZE) = _CHUNK_SIZE;
    *(unsigned int *) &p_settings->MAX_TICKRATE = _MAX_TICKRATE;
    *(unsigned short *) &p_settings->MAX_PLAYERS_PER_ROOM = _MAX_PLAYERS_PER_ROOM;

    return 0;
}