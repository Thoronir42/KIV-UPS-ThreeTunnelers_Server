#ifndef CMD_QUEUE_H
#define CMD_QUEUE_H

#include "../networks/network_command.h"

#define QUEUE_SIZE 42

typedef struct cmd_queue
{
    pthread_mutex_t mutex;
    network_command commands[QUEUE_SIZE];
    const int size;
    int put_index;
    int get_index;
} cmd_queue;

void cmd_queue_init(cmd_queue *p);

int cmd_queue_is_empty(cmd_queue *p);

int cmd_queue_is_full(cmd_queue *p);

network_command cmd_queue_get(cmd_queue *p);

void cmd_queue_put(cmd_queue *p, network_command cmd);

#endif /* CMD_QUEUE_H */

