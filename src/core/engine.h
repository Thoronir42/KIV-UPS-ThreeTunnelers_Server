#ifndef ENGINE_H
#define ENGINE_H

#include <stdlib.h>
#include <stddef.h>

#include "resources.h"
#include "../networks/netadapter.h"
#include "../networks/network_command.h"
#include "../structures/cmd_queue.h"
#include "../settings.h"
#include "../statistics.h"
#include "../generic.h"
#include "str_scanner.h"

#define ENGINE_CLI_BUFFER_SIZE 24

#define ENGINE_CMDEXE_OK 0
#define ENGINE_CMDEXE_DATA_TOO_SHORT 1
#define ENGINE_CMDEXE_WRONG_CONTEXT 2
#define ENGINE_CMDEXE_ILLEGAL_OP 3

#define ENGINE_CLOSE_REASON_ACTION_NOT_IMPLEMENTED 1
#define ENGINE_CLOSE_REASON_TOO_MANY_INVALIDES 2


#define ENGINE_HANDLE_FUNC_HEADER (struct engine* p, net_client *p_cli, str_scanner* sc, game_room *p_cgr)

typedef struct engine
{
    settings *settings;
    resources *resources;
    netadapter netadapter;
    statistics stats;

    struct timespec sleep;
    unsigned long current_tick;

    int keep_running;

    cmd_queue cmd_in_queue;
    int (*command_proccess_func[NETWORK_COMMAND_TYPES_COUNT])ENGINE_HANDLE_FUNC_HEADER;

    network_command _cmd_out;

    netadapter *p_netadapter;
    network_command *p_cmd_out;

    void(* game_room_update_functions[GAME_ROOM_STATES_COUNT])(struct engine *, game_room *);
    int game_room_update_tickskip[GAME_ROOM_STATES_COUNT];

} engine;

int engine_init(engine *p_engine, settings *p_settings, resources *p_resources);
void _engine_init_gameroom_updates(engine *p);

void _engine_init_solo_commands(int (**command_handle_func)ENGINE_HANDLE_FUNC_HEADER);
void _engine_init_gameroom_commands(int (**command_handle_func)ENGINE_HANDLE_FUNC_HEADER);
void _engine_init_game_play_commands(int (**command_handle_func)ENGINE_HANDLE_FUNC_HEADER);

void engine_handle_command(void *handler, const network_command cmd);

void engine_update_gamerooms(engine *p);

void *engine_run(void *args);
void *engine_cli_run(void *args);

void engine_client_disconnected(engine *p, net_client *p_cli, char *reason);

int engine_count_clients(engine *p, net_client_status status);

net_client *engine_first_free_client_offset(engine *p);
net_client *engine_client_by_socket(engine *p, int socket);
net_client *engine_client_by_secret(engine *p, char *secret);

game_room *engine_game_room_by_id(engine *p, int room_id);
game_room *engine_find_empty_game_room(engine *p);

void engine_send_command(engine *p, net_client *p_cli, network_command *cmd);
void engine_bc_command(engine *p, game_room *p_gr, network_command *cmd);
void engine_bc_command_status_filter(engine *p, game_room *p_gr, network_command *cmd, net_client_status status);

void engine_game_room_put_client(engine *p, game_room *p_gr, net_client *p_cli);
void engine_game_room_put_client_again(engine *p, game_room *p_gr, net_client *p_cli);
void engine_game_room_client_disconnected(engine *p, game_room *p_gr, net_client *p_cli, char *reason);
void engine_game_room_set_state(engine *p, game_room *p_gr, game_room_state game_state);
void engine_game_room_dump_map_to_client(engine *p, net_client *p_cli, tunneler_map *p_map);
void engine_game_room_dump_to_client(engine *p, game_room *p_gr, net_client *p_cli);
int engine_game_room_put_player(engine *p, game_room *p_gr, net_client *p_cli);
void engine_game_room_remove_player(engine *p, game_room *p_gr, int player_rid);

// network command related stuff
void engine_gameroom_process_map_changes(engine *p, game_room *p_gr);
void engine_gameroom_sync_tanks(engine *p, game_room *p_gr);
void engine_gameroom_tank_destroyed(engine *p, game_room *p_gr, tank *p_tank);

#endif
