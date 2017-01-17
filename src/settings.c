#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "settings.h"


int settings_process_arguments(settings *p, int argc, char *argv[]) {
    //dump_args(argc, argv);
    if (argc > 1 && strcmp(argv[1], "test") == 0) {
        return ARGERR_RUN_TESTS;
    }
    if (argc < 3) {
        return ARGERR_NOT_ENOUGH_ARGUMENTS;
    }
    p->show_statistics = 1;
    p->port = atoi(argv[ARG_REQ_PORT]);
    if(p->port == 0 || p->port > ARG_PORT_MAX_VALUE){
        return ARGERR_INVALID_PORT;
    }

    *(int *) &p->MAX_ROOMS = atoi(argv[ARG_MAX_ROOMS]);
    if(p->MAX_ROOMS == 0 || p->MAX_ROOMS > ARG_ROOMS_MAX_VALUE){
        return ARGERR_INVALID_ROOM_AMOUNT;
    }
//    printf("ARG: rooms = %d\n", p->MAX_ROOMS);
    *(int *) (&p->CHUNK_SIZE) = _CHUNK_SIZE;
    *(unsigned int *) &p->MAX_TICKRATE = _MAX_TICKRATE;
    *(unsigned short *) &p->MAX_PLAYERS_PER_ROOM = _MAX_PLAYERS_PER_ROOM;

    return 0;
}