#ifndef RUN_SUMMARY_H
#define	RUN_SUMMARY_H

#include <time.h>

typedef struct run_summary{
	clock_t run_start, run_end;
	unsigned int udp_sent, udp_received, udp_received_invalid;
} run_summary;

run_summary *run_summary_create();

void run_summary_delete(run_summary *p);


#endif	/* RUN_SUMMARY_H */

