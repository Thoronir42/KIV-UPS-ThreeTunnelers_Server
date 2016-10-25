
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "summary.h"

int summary_init(summary *p) {
    memset(p, 0, sizeof(summary));

    return 0;
}

void summary_print(summary *p) {
    printf("ThreeTunnelers runtime summary\n");
    printf("Run length: %d\n", p->run_end - p->run_start);

    printf("Network statisticcs\n");
    printf("Total TCP messages sent by server            : %d\n", p->commands_sent);
    printf("Total TCP messages received [total(invalid)] : %d(%d)\n", p->commands_received, p->commands_received_invalid);

}