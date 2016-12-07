#include <string.h>
#include <stdio.h>

#include "engine.h"

_list_page_count(int items, int ipp){
    return items / ipp + (items % ipp > 0 ? 1 : 0);
}

void _cli_shutdown(engine *p) {
    p->keep_running = 0;
}

void _cli_list_clients(netadapter *p) {
    int i, n = 0, cpp = 20;
    net_client *p_client;
    time_t now = time(NULL);
    int idle;
    char status;

    int pages = _list_page_count(p->clients_length, cpp);

    printf("Clients listing: \n");

    for (i = 0; i < p->clients_length; i++) {
        if (i % cpp == 0) {
            printf("╒════╤════╤══════════════╤═════╤════════╤═════════╕\n");
            printf("│ ID │ soc│ Name         │ STS │ Idle T │ %02d / %02d │\n", (i / cpp) + 1, pages);
            printf("╞════╪════╪══════════════╪═════╪════════╪═════════╛\n");
        }
        p_client = p->clients + i;
        if (p_client->status != NET_CLIENT_STATUS_EMPTY) {
            //idle = (now - p_client->last_active);
            idle = now - p_client->connection.last_active;
            n++;
        } else {
            idle = 0;
        }
        status = client_status_letter(p_client->status);
        printf("│ %02d │ %02d │ %12s │   %c │ %6d │\n",
                i, p_client->connection.socket, p_client->name,
                status, idle);
    }

    printf("Total connected clients: %02d\n", n);
}

void _cli_list_connections(netadapter *p) {
    int i, n = 0, cpp = 20;
    tcp_connection *p_con;
    time_t now = time(NULL);
    int idle;
    char status;

    int pages = _list_page_count(p->connections_length, cpp);

    printf("Temp connections listing: \n");

    for (i = 0; i < p->clients_length; i++) {
        if (i % cpp == 0) {
            printf("╒════╤════╤════════╤═════════╕\n");
            printf("│ ID │ soc│ Idle T │ %02d / %02d │\n", (i / cpp) + 1, pages);
            printf("╞════╪════╪════════╪═════════╛\n");
        }
        p_con = p->connections + i;
        idle = now - p_con->last_active;
        
        printf("│ %02d │ %02d │ %6d │\n",
                i, p_con->socket, idle);
    }

    printf("Total active temp connections: %02d\n", n);
}

void *engine_cli_run(void *args) {
    engine *p_engine = (engine *) args;
    char input[ENGINE_CLI_BUFFER_SIZE];
    while (p_engine->keep_running) {
        printf("TTS >: ");
        memset(input, 0, ENGINE_CLI_BUFFER_SIZE);
        scanf("%s", input);

        if (!strcmp(input, "exit")) {
            _cli_shutdown(p_engine);
        } else if (!strcmp(input, "clients")) {
            _cli_list_clients(&p_engine->netadapter);
        } else if (!strcmp(input, "connections")) {
            _cli_list_clients(&p_engine->netadapter);
        }
    }

    printf("Engine CLI: Exittig\n");
    return NULL;
}