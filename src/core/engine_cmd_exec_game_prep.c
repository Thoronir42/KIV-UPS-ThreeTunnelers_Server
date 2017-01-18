#include <stdlib.h>
#include <stdio.h>

#include "engine.h"

#include "../logger.h"

int _exe_prep_msg_plain ENGINE_HANDLE_FUNC_HEADER
{   
    game_room *p_gr;
    int clientRID;
    
    p_gr = engine_game_room_by_id(p, p_cli->room_id); // todo: null check
    if(p_gr == NULL){
        return ENGINE_CMDEXE_WRONG_CONTEXT;
    }
    clientRID = game_room_find_client(p_gr, p_cli);
    
    network_command_prepare(p->p_cmd_out, NCT_MSG_PLAIN);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_str(p->p_cmd_out, sc->str, sc->length);
    
    netadapter_broadcast_command_p(p->p_netadapter, p_gr->clients, GAME_ROOM_MAX_PLAYERS, p->p_cmd_out);
}

void _engine_init_game_prep_commands(int (**actions)ENGINE_HANDLE_FUNC_HEADER){
    actions[NCT_MSG_PLAIN] = &_exe_prep_msg_plain;
    
    glog(LOG_WARNING, "NOT IMPLEMENTED YET");
}
