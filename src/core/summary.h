#ifndef SUMMARY_H
#define SUMMARY_H

#include <time.h>

typedef struct summary {
    clock_t run_start, run_end;
    unsigned int commands_sent, commands_received, commands_received_invalid;
} summary;

int summary_init(summary *p);

void summary_print(summary *p);


#endif /* SUMMARY_H */

