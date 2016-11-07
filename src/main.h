#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>

#include "core/engine.h"

#define MAIN_ERR_BAD_ARGS 1
#define MAIN_ERR_NETWORK_FAILED 2

#define THR_ENGINE 0
#define THR_ENGINE_CLI 1
#define THR_NETADAPTER 2


void print_help(const char *file, int err);

void main_run_threads(pthread_t *threads, engine *p_engine);


#endif /* MAIN_H */

