#include <string.h>
#include <stdio.h>

#include "engine.h"
#include "../logger.h"

#define STATUS_VALUE_SIZE 24

_list_page_count(int items, int ipp) {
    return items / ipp + (items % ipp > 0 ? 1 : 0);
}

void _cli_shutdown(engine *p) {
    p->keep_running = 0;
}

void _cli_status(engine *p) {
    char * format = " %16s : %s\n";
    char value[STATUS_VALUE_SIZE];

    clock_t now = clock();

    int run_length = (now - p->stats.run_start) / 1000;
    int run_mins = run_length / 60;
    int run_sec = run_length % 60;

    memset(value, '\0', STATUS_VALUE_SIZE);
    sprintf(value, "%3d:%02d\n", run_mins, run_sec);
    printf(format, "Uptime", value);

    memset(value, '\0', STATUS_VALUE_SIZE);
    sprintf(value, "%3d / %3d", engine_count_clients(p, NET_CLIENT_STATUS_CONNECTED), p->resources->clients_length);
    printf(format, "Current clients", value);

}

void _cli_list_clients(netadapter *p) {
    int i, n = 0, cpp = 20;
    net_client *p_client;
    time_t now = time(NULL);
    int idle;
    char status;
    tcp_connection empty_connection;
    tcp_connection *p_con;

    int pages = _list_page_count(p->clients_length, cpp);
    memset(&empty_connection, 0, sizeof (tcp_connection));
    empty_connection.last_active = now;

    printf("Clients listing: \n");

    for (i = 0; i < p->clients_length; i++) {
        if (i % cpp == 0) {
            printf("╒════╤════╤══════════════╤═════╤════════╤═════════╕\n");
            printf("│ ID │ soc│ Name         │ STS │ Idle T │ %02d / %02d │\n", (i / cpp) + 1, pages);
            printf("╞════╪════╪══════════════╪═════╪════════╪═════════╛\n");
        }
        p_client = p->clients + i;
        p_con = p_client->connection != NULL ? p_client->connection : &empty_connection;

        if (p_client->status != NET_CLIENT_STATUS_EMPTY) {
            idle = now - p_con->last_active;
            n++;
        } else {
            idle = 0;
        }
        status = net_client_status_letter(p_client->status);
        printf("│ %02d │ %02d │ %12s │   %c │ %6d │\n",
                i, p_con->socket, p_client->name,
                status, idle);
    }

    printf("Total connected clients: %02d\n", n);
}

void _cli_list_connections(netadapter *p) {
    int i, n = 0, cpp = 20;
    tcp_connection *p_con;
    time_t now = time(NULL);
    int idle;

    int pages = _list_page_count(p->connections_length, cpp);

    printf("Temp connections listing: \n");

    for (i = 0; i < p->connections_length; i++) {
        if (i % cpp == 0) {
            printf("╒════╤════╤════╤════════╤═════════╕\n");
            printf("│ ID │ soc│ cli│ Idle T │ %02d / %02d │\n", (i / cpp) + 1, pages);
            printf("╞════╪════╪════╪════════╪═════════╛\n");
        }
        p_con = p->connections + i;
        idle = now - p_con->last_active;
        if (p_con->socket != NETADAPTER_SOCKET_EMPTY) {
            n++;
        } else {
            idle = 0;
        }

        printf("│ %02d │ %02d │ %02d │ %6d │\n",
                i, p_con->socket, p->connection_to_client[i], idle);
    }

    printf("Total open connections: %02d\n", n);
}

void _cli_list_rooms(engine *p) {
    int items_per_page = 6, pages;

    int i, j, size, clients, players;
    char status;
    char *client_name;



    game_room *p_gr;

    pages = _list_page_count(p->resources->game_rooms_length, items_per_page);
    for (i = 0; i < p->resources->game_rooms_length; i++) {
        if (i % items_per_page == 0) {
            printf(""
                    "╒════╤═══╤═════╤═════╤═════╤═════════╕\n"
                    "│ ID │STS│ SIZ │ CLI │ PLR │ %02d / %02d │\n"
                    "╞════╪═══╪═════╪═════╪═════╪═════════╛\n",
                    (i / items_per_page) + 1, pages);
        }
        p_gr = p->resources->game_rooms + i;

        status = game_room_status_letter(p_gr->state);
        clients = game_room_count_clients(p_gr);
        players = game_room_count_players(p_gr);


        printf("│ %02d │ %c │ %3d │ %3d │ %3d │\n",
                i, status, p_gr->size, clients, players);
        if (p_gr->state == GAME_ROOM_STATE_IDLE) {
            continue;
        }

        printf(""
                "└┬───┴───┴─────┴─────┴─┬───┤\n"
                " │Clients              │Plr│\n"
                " ├─────────────────────┼───┤\n");
        for (j = 0; j < p_gr->size; j++) {
            client_name = p_gr->clients[j] == NULL ? "---" : p_gr->clients[j]->name;
            printf(" │%21s│", client_name);
            if (p_gr->players[j].client_rid == ITEM_EMPTY) {
                printf("---│\n");
            } else {
                printf("%3d│\n", p_gr->players[j].client_rid);
            }

        }
        printf("┌┴───┬───┬─────┬─────┬─┴───┤\n");
    }

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
            _cli_list_connections(&p_engine->netadapter);
        } else if (!strcmp(input, "status")) {
            _cli_status(p_engine);
        } else if (!strcmp(input, "rooms")) {
            _cli_list_rooms(p_engine);
        }
    }

    glog(LOG_FINE, "Engine CLI: Exittig");
    return NULL;
}