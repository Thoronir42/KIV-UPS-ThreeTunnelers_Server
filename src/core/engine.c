#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "engine.h"

#include "../networks/netadapter.h"

#include "../settings.h"
#include "summary.h"
#include "../game/control_input.h"

#define CLOCK CLOCK_MONOTONIC

int engine_init(engine *p_engine, struct settings *p_settings) {
    memset(p_engine, 0, sizeof (engine));
    p_engine->settings = p_settings;

    unsigned long milliseconds = 1000 / p_settings->MAX_TICKRATE;
    
    printf("Engine: Sleep millis = %lu\n", milliseconds);

    p_engine->sleep.tv_sec = milliseconds / 1000;
    p_engine->sleep.tv_nsec = (milliseconds % 1000) * 1000000;
    
    
    printf("Engine: Initialising netadapter\n");
    if (netadapter_init(&p_engine->netadapter, p_settings->port)) {
        return ENGERR_NETWORK_INIT_FAILED;
    }

    printf("Engine: Initialising summaries\n");
    summary_init(&p_engine->summary);

    return 0;
}

void *engine_run(void *args) {
    engine *p_engine = (engine *) args;

    p_engine->keep_running = 1;
    p_engine->summary.run_start = clock();
    printf("Engine: Starting\n");
    while (p_engine->keep_running) {
        p_engine->total_ticks++;
        if (!(p_engine->total_ticks % p_engine->settings->MAX_TICKRATE))
            printf("Engine runs * %lu\n", p_engine->total_ticks / p_engine->settings->MAX_TICKRATE);
        if (p_engine->total_ticks > p_engine->settings->MAX_TICKRATE * 5) {
            printf("Engine ends after %lu ticks.\n", p_engine->total_ticks);
            p_engine->keep_running = 0;
        }
        nanosleep(&p_engine->sleep, NULL);
    }

    p_engine->summary.run_end = clock();
    
    printf("Engine: Exitting\n");
    return NULL;
}

void *engine_input_run(void *args) {
    engine *p_engine = (engine *) args;
    char input[20];
    while (p_engine->keep_running) {
        read(STDIN_FILENO, &input, 20);
        printf("%c\n", input);
    }
    return NULL;
}