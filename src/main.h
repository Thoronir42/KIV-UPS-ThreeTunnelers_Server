#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>

#include "core/engine.h"

#define MAIN_ERR_TEST_RUN 1
#define MAIN_ERR_BAD_ARGS 2
#define MAIN_ERR_RES_ALLOCATION_FAIL 3
#define MAIN_ERR_NETWORK_FAILED 4

#define THR_ENGINE 0
#define THR_ENGINE_CLI 1
#define THR_NETADAPTER 2


void print_help(const char *file, int err);

int main_startup(int argc, char *argv[], settings *p_settings, resources *p_resources);
int main_run(settings *p_settings, resources *p_resources);


#endif /* MAIN_H */

