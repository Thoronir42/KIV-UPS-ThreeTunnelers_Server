#include <stdlib.h>
#include <stdio.h>

#include "engine.h"

#include "../logger.h"


int _exe_gpl_controls_set ENGINE_HANDLE_FUNC_HEADER
{
    my_byte control_state;
    int player_rid, current_client_rid, changed;
    player *p_plr;

    if (sc->length < 4) {
        return ENGINE_CMDEXE_DATA_TOO_SHORT;
    }
    if (p_cgr == NULL) {
        return ENGINE_CMDEXE_WRONG_CONTEXT;
    }

    current_client_rid = game_room_find_client(p_cgr, p_cli);
    if (current_client_rid == ITEM_EMPTY) {
        return ENGINE_CMDEXE_ILLEGAL_OP;
    }

    player_rid = strsc_byte(sc);
    control_state = strsc_byte(sc);
    
    p_plr = game_room_get_player(p_cgr, player_rid);
    if (p_plr == NULL || p_plr->client_rid != current_client_rid) {
        glog(LOG_INFO, "Room %d: Client %d tried to change controls of player %d",
                p_cgr - p->resources->game_rooms, current_client_rid, player_rid);
        return ENGINE_CMDEXE_ILLEGAL_OP;
    }

    changed = controls_set_state(&p_plr->input, control_state);

    network_command_prepare(p->p_cmd_out, NCT_GAME_CONTROLS_SET);
    network_command_append_byte(p->p_cmd_out, player_rid);
    network_command_append_byte(p->p_cmd_out, control_state);
    
    if (changed) {
        engine_bc_command(p, p_cgr, p->p_cmd_out);
    } else {
        engine_send_command(p, p_cli, p->p_cmd_out);
    }

    return 0;

}

int _exe_gpl_tank_info ENGINE_HANDLE_FUNC_HEADER{
    int playerRID;
    tank *p_tank;
    if (sc->length < 2) {
        return ENGINE_CMDEXE_DATA_TOO_SHORT;
    }
    if (p_cgr == NULL || p_cgr->state == GAME_ROOM_STATE_BATTLE) {
        return ENGINE_CMDEXE_WRONG_CONTEXT;
    }
    playerRID = strsc_byte(sc);


    if (game_room_get_player(p_cgr, playerRID) == NULL) {
        return ENGINE_CMDEXE_ILLEGAL_OP;
    }
    p_tank = warzone_get_tank(&p_cgr->zone, playerRID);


    network_command_prepare(p->p_cmd_out, NCT_GAME_TANK_INFO);




}

void _engine_init_game_play_commands(int (**actions)ENGINE_HANDLE_FUNC_HEADER) {
    actions[NCT_GAME_CONTROLS_SET] = &_exe_gpl_controls_set;
    actions[NCT_GAME_TANK_INFO] = &_exe_gpl_controls_set;
}
