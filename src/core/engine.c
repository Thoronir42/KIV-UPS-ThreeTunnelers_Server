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
    
    p_engine->netadapter.status = NETADAPTER_STATUS_SHUTTING_DOWN;

    p_engine->summary.run_end = clock();

    printf("Engine: Exitting\n");
    return NULL;
}
