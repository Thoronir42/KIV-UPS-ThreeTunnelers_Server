#include <string.h>
#include <stdio.h>

#include "engine.h"

void _engine_cli_shutdown(engine *p) {
    p->keep_running = 0;
}

void _engine_cli_list_clients(engine *p) {
    int i, n = 0;
    net_client *p_client;
    clock_t now = clock();
    double idle;
	
    printf("Currently logged clients: \n");
	netadapter_check_clients(&p->netadapter);
	return;
	
    for (i = 0; i < p->netadapter.clients_size; i++) {
        p_client = p->netadapter.clients + i;
        if ((p_client)->status != NET_CLIENT_STATUS_EMPTY) {
            idle = (now - p_client->last_active);
            
            printf("Client %02d: %12s ", i, p_client->name);
            printf(" connected:%s", p_client->status == NET_CLIENT_STATUS_CONNECTED ? "1" : "0");
            printf(" idle time: %02.01f", idle);
            printf("\n");
            n++;
        }
    }
    
    printf("Total connected clients: %02d\n", n);
}

void *engine_cli_run(void *args) {
    engine *p_engine = (engine *) args;
    char input[ENGINE_CLI_BUFFER_SIZE];
    while (p_engine->keep_running) {
        printf("TTS >: ");
        memset(input, 0, ENGINE_CLI_BUFFER_SIZE);
        scanf("%s", input);

        if (!strcmp(input, "exit")) {
            _engine_cli_shutdown(p_engine);
        } else if (!strcmp(input, "clients")) {
            _engine_cli_list_clients(p_engine);
        }
    }

    printf("Engine CLI: Exittig\n");
    return NULL;
}