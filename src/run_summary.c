
#include <stdlib.h>
#include <stdio.h>

#include "run_summary.h"

run_summary *run_summary_create(){
	run_summary *tmp = malloc(sizeof(run_summary));
	tmp->udp_received = tmp->udp_sent = 0;
	
	return tmp;
	
}

void run_summary_delete(run_summary *p){
	free(p);
}


void run_summary_print(run_summary *p){
	printf("ThreeTunnelers runtime summary\n");
	printf("Run length: %d\n", p->run_end - p->run_start);
	
	printf("Network statisticcs\n");
	printf("Total UDP packets sent by server: %d\n", p->udp_sent);
	printf("Total UDP packets received [total(unrecognised)]: %d(%d)\n", p->udp_received, p->udp_received_invalid);
	
}