#ifndef ENGINE_H
#define ENGINE_H

#include <stdlib.h>
#include <stddef.h>

#include "../data/resources.h"
#include "../networks/netadapter.h"
#include "../settings.h"
#include "summary.h"

#define ENGERR_NETWORK_INIT_FAILED 1

typedef struct engine {
	settings *settings;
	resources *resources;
	netadapter netadapter;
	summary summary;

	struct timespec sleep;
	unsigned long total_ticks;

	int keep_running;



} engine;

int engine_init(engine *p_engine, settings *p_settings, resources *p_resources);

void *engine_run(void *args);

void *engine_input_run(void *args);


#endif
