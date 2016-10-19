#ifndef ENGINE_H
#define ENGINE_H



#include <stdlib.h>
#include <stddef.h>

#include "../networks/networks.h"
#include "../settings.h"
#include "run_summary.h"

typedef struct engine {
	networks *p_networks;
	settings *p_settings;
	run_summary *p_summary;

	useconds_t usleep_length;
	unsigned long total_ticks;

	int keep_running;



} engine;

engine *engine_create(networks *p_networks, settings *p_settings);

void engine_delete(engine *p_engine);

void *engine_run(void *args);

void *engine_input_run(void *args);


#endif
