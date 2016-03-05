#ifndef ENGINE_H
#define ENGINE_H

#include <stdlib.h>
#include <stddef.h>

#include "networks/networks.h"
#include "settings.h"

typedef struct engine{
	networks *p_networks;
	settings *p_settings;
	
	int keep_running;
	
} engine;

engine *engine_create(networks *p_networks, settings *p_settings);

void engine_delete(engine *e);

void *engine_run();



#endif
