#include <stdlib.h>
#include <stdio.h>

#include "engine.h"

#include "../logger.h"


int _exe_gpl_controls_set ENGINE_HANDLE_FUNC_HEADER
{
    my_byte control_state;
    int playerRID, clientRID, changed;
    player *p_plr;

    if (sc->length < 4) {
        return ENGINE_CMDEXE_DATA_TOO_SHORT;
    }
    if (p_cgr == NULL) {
        return ENGINE_CMDEXE_WRONG_CONTEXT;
    }

    clientRID = game_room_find_client(p_cgr, p_cli);
    if (clientRID == ITEM_EMPTY) {
        return ENGINE_CMDEXE_ILLEGAL_OP;
    }

    playerRID = strsc_byte(sc);
    control_state = strsc_byte(sc);

    p_plr = game_room_get_player(p_cgr, playerRID);
    if (p_plr == NULL || p_plr->client_rid != clientRID) {
        return ENGINE_CMDEXE_ILLEGAL_OP;
    }

    changed = input_set_state(&p_plr->input, control_state);
    if (changed) {
        network_command_prepare(p->p_cmd_out, NCT_GAME_CONTROLS_SET);
        network_command_append_byte(p->p_cmd_out, playerRID);
        network_command_append_byte(p->p_cmd_out, control_state);
        engine_bc_command(p, p_cgr, p->p_cmd_out);
    }

    return 0;

}

int _exe_gpl_tank_info ENGINE_HANDLE_FUNC_HEADER{
    int playerRID;
    tank *p_tank;
    if (sc->length < 2) {
        return ENGINE_CMDEXE_DATA_TOO_SHORT;
    }
    if (p_cgr == NULL || p_cgr->state == GAME_ROOM_STATE_RUNNING) {
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
