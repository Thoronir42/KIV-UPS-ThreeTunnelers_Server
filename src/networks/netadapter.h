#ifndef NETADAPTER_H
#define NETADAPTER_H

typedef struct netadapter {
    int socket;
} netadapter;

int netadapter_init(netadapter *p);

int netadapter_start(netadapter *p);

int netadapter_destroy(netadapter *p);

#endif /* NETADAPTER_H */

