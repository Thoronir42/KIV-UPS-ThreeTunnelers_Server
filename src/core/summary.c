#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "summary.h"

int summary_init(summary *p) {
    memset(p, 0, sizeof(summary));

    return 0;
}

void summary_print(summary *p) {
    int run_length = (p->run_end - p->run_start) / 1000;
    int run_mins = run_length / 60;
    int run_sec = run_length % 60;
    
    printf("ThreeTunnelers runtime summary\n");
    printf("Run length: %3d:%02d\n", run_mins, run_sec);

    printf("\
┌─────┬───────┬───────┬───────┐\n\
│ TCP │ sent  │ rcv-ok│ rcv-er│\n\
└─────┼───────┼───────┼───────┤\n\
      │%7d│%7d│%7d│\n\
      └───────┴───────┴───────┘",
            p->commands_sent, p->commands_received, p->commands_received_invalid);

}