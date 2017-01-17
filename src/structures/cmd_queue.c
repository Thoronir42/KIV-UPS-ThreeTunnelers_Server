#include <stdlib.h>
#include <pthread.h>

#include "cmd_queue.h"

#include "../networks/network_command.h"

void cmd_queue_init(cmd_queue *p) {
    pthread_mutex_init(&p->mutex, NULL);
    *(int *)&p->size = QUEUE_SIZE;
}

int cmd_queue_is_empty(cmd_queue *p) {
    return p->get_index == p->put_index;
}

int cmd_queue_is_full(cmd_queue *p) {
    return p->put_index % p->size == (p->get_index - 1) % p->size;
}

network_command cmd_queue_get(cmd_queue *p) {
    network_command cmd;

    pthread_mutex_lock(&p->mutex);

    cmd = p->commands[p->get_index];
    p->get_index = (p->get_index + 1) % p->size;

    pthread_mutex_unlock(&p->mutex);

    return cmd;
}

void cmd_queue_put(cmd_queue *p, network_command cmd) {
    pthread_mutex_lock(&p->mutex);

    p->commands[p->put_index] = cmd;
    p->put_index = (p->put_index + 1) % p->size;

    pthread_mutex_unlock(&p->mutex);
}