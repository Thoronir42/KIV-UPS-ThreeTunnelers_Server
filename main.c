#include<stddef.h>
#include <stdlib.h>

#include "settings.h"
#include "networks/networks.h"
#include "engine.h"

#define ARG_MAX_ROOMS 0


int process_arguments(int argc, char *argv[], settings *p_settings) {
	int i;
	for (i = 0; i < argc; i++) {
		printf(argv[i]);
	}
	
	int max_rooms = atoi(argv[ARG_MAX_ROOMS])
	*(int *) &p_settings->MAX_ROOMS = max_rooms;

	return 0;
}

int main(int argc, char* argv[]) {
	settings *p_settings = malloc(sizeof (settings));

	process_arguments(argc - 1, argv + 1, p_settings);
	
	networks *p_networks = networks_create(p_settings);
	
	
	engine *p_engine = engine_create(p_networks, p_settings);
	
	p_engine->keep_running = 0;

	free(p_settings);
	
	return 0;
}
