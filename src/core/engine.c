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
#include "src/game/input.h"

#define CLOCK CLOCK_MONOTONIC

int engine_init(engine *p_engine, settings *p_settings) {
	memset(p_engine, 0, sizeof (engine));

	p_engine->settings = *p_settings;

	if (netadapter_init(&p_engine->netadapter, p_settings)) {
		return ENGERR_NETWORK_INIT_FAILED;
	}

	summary_init(&p_engine->summary);

	p_engine->total_ticks = 0;
	p_engine->usleep_length = 1000000 / p_settings->MAX_TICKRATE;

	return 0;
}

void engine_delete(engine *p_engine) {
	netadapter_destroy(&p_engine->netadapter);

	free(p_engine);
}

void *engine_run(void *args) {
	engine *p_engine = (engine *) args;

	p_engine->keep_running = 1;
	p_engine.summary->run_start = clock();
	while (p_engine->keep_running) {
		p_engine->total_ticks++;
		if (!p_engine->total_ticks % p_engine->settings->MAX_TICKRATE)
			printf("Engine runs * %lu\n", p_engine->total_ticks / p_engine->settings->MAX_TICKRATE);
		if (p_engine->total_ticks > p_engine->settings->MAX_TICKRATE * 5) {
			printf("Engine ends after %lu ticks.\n", p_engine->total_ticks);
			p_engine->keep_running = 0;
		}

		usleep(p_engine->usleep_length);
	}

	p_engine->summary->run_end = clock();

	if (p_engine->settings->show_summaries) {
		printf("Engine has ended, printing sumamry: \n");
		summary_print(p_engine->summary);
	}

	return NULL;
}

void *engine_input_run(void *args) {
	engine *p_engine = (engine *) args;
	char[20] input;
	while (p_engine->keep_running) {
		read(stdin, &input, 20);
		printf(input);
	}
	return NULL;
}