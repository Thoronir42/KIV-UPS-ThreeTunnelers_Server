#ifndef ENGINE_H
#define ENGINE_H



#include <stdlib.h>
#include <stddef.h>

#include "networks/networks.h"
#include "settings.h"
#include "run_summary.h"

typedef struct engine{
	networks *p_networks;
	settings *p_settings;
	
	useconds_t usleep_length;
	unsigned long total_ticks;
	
	int keep_running;
	
	run_summary *p_summary;
	
} engine;

engine *engine_create(networks *p_networks, settings *p_settings);

void engine_delete(void *args);

void *engine_run(void *args);

void *engine_key_input_run(void *args);


#endif
