#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


#include "settings.h"

#define ARG_REQ_PORT 0
#define ARG_MAX_ROOMS 1

int settings_process_arguments(settings *p_settings, int argc, char *argv[]) {
	if(argc < 2){
		return 1;
	}
	
	
	p_settings->show_summaries = 1;
	
	
	
	int max_rooms = atoi(argv[ARG_MAX_ROOMS]);
	*(int *) &p_settings->MAX_ROOMS = max_rooms;
	
	int req_port = atoi(argv[ARG_MAX_ROOMS]);

	return p_settings;
}