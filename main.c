#include<stddef.h>
#include <stdlib.h>

#include "settings.h"
#include "networkz.h"

#define ARG_MAX_ROOMS 0


int process_arguments(int argc, char *argv[], settings *p_settings) {
	int i;
	for (i = 0; i < argc; i++) {
		printf(argv[i]);
	}

	*((int *)(&p_settings->MAX_ROOMS)) = atoi(argv[ARG_MAX_ROOMS]);

}

int main(int argc, char* argv[]) {
	settings *p_settings = malloc(sizeof (settings));

	process_arguments(argc - 1, argv + 1, p_settings);


	free(p_settings);
}
