#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


#include "settings.h"

#define ARG_MAX_ROOMS 0

settings *settings_process_arguments(int argc, char *argv[]) {
	settings *p_settings = malloc(sizeof(settings));
	
	int i;
	for (i = 0; i < argc; i++) {
		printf(argv[i]);
	}
	
	int max_rooms = atoi(argv[ARG_MAX_ROOMS]);
	*(int *) &p_settings->MAX_ROOMS = max_rooms;

	return p_settings;
}