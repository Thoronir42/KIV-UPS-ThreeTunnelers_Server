#include <stdlib.h>
#include <stdio.h>

#include "engine.h"

#include "../logger.h"

int _exe_prep_msg_plain ENGINE_HANDLE_FUNC_HEADER
{   
    // todo: implement
    game_room *p_gr;
    int clientRID;
    
    p->p_cmd_out->type = NCT_MSG_PLAIN;
    
    p_gr = engine_room_by_client(p, p_cli); // todo: null check
    clientRID = game_room_find_client(p_gr, p_cli);
    
    
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_str(p->p_cmd_out, sc->str, sc->length);
    
    netadapter_broadcast_command_p(p->p_netadapter, p_gr->clients, GAME_ROOM_MAX_PLAYERS, p->p_cmd_out);
}

void _engine_init_game_prep_commands(int (**command_handle_func)ENGINE_HANDLE_FUNC_HEADER){
    glog(LOG_WARNING, "NOT IMPLEMENTED YET");
}
