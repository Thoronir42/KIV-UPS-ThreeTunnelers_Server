#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


#include "settings.h"

#define ARG_REQ_PORT 0
#define ARG_MAX_ROOMS 1

int settings_process_arguments(settings *p_settings, int argc, char *argv[]) {
	if(argc - 1 < 2){
		return 1;
	}
	char **rargv = argv + 1;
	
	
	p_settings->show_summaries = 1;
	
	int req_port = atoi(rargv[ARG_MAX_ROOMS]);
	p_settings->port = req_port;
	
	int max_rooms = atoi(rargv[ARG_MAX_ROOMS]);
	*(int *) &p_settings->MAX_ROOMS = max_rooms;
	
	
	
	
	return 0;
}