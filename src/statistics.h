#ifndef STATISTICS_H
#define STATISTICS_H

#include <time.h>

typedef struct statistics {
    clock_t run_start, run_end;
    unsigned int commands_sent, commands_received, commands_received_invalid;
    unsigned long bytes_sent, bytes_received;
} statistics;

int statistics_init(statistics *p);

void statistics_print(statistics *p);


#endif /* SUMMARY_H */

