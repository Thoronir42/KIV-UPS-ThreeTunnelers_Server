#include<stddef.h>
#include <stdlib.h>

#include "settings.h"
#include "networks/networks.h"
#include "engine.h"




int main(int argc, char* argv[]) {
	settings *p_settings = settings_process_arguments(argc - 1, argv + 1);
	
	networks *p_networks = networks_create(p_settings);
	engine *p_engine = engine_create(p_networks, p_settings);
	
	p_engine->keep_running = 0;

	free(p_settings);
	
	return 0;
}
