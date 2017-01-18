#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "engine.h"

#include "../logger.h"
#include "../localisation.h"

int _exe_solo_undefined ENGINE_HANDLE_FUNC_HEADER
{
    return 1;
}

int _exe_solo_lead_disconnect ENGINE_HANDLE_FUNC_HEADER{
    netadapter_close_connection_by_client(p->p_netadapter, p_cli);
    p_cli->status = NET_CLIENT_STATUS_DISCONNECTED;
    game_room *p_gr = engine_room_by_client(p, p_cli);

    if (p_gr) {
        p->p_cmd_out->type = NCT_ROOM_CLIENT_REMOVE;
        network_command_set_data(p->p_cmd_out, g_loc.client_disconnected, strlen(g_loc.client_disconnected));
        netadapter_broadcast_command_p(p->p_netadapter, p_gr->clients, p_gr->size, p->p_cmd_out);
    }

    return 0;
}

int _exe_solo_lead_marco ENGINE_HANDLE_FUNC_HEADER{
    glog(LOG_FINE, "Scanner content[%02d/%02d]: %s", sc->read, sc->length, sc->str);
    if (sc->length < 18) { // 2=n + 16=stamp
        glog(LOG_WARNING, "Received MARCO command with too short body");
        return 1;
    }
    int n = strsc_byte(sc);
    long their_stamp = strsc_long(sc);

    glog(LOG_FINE, "Marco timestamp = %l", their_stamp);
    glog(LOG_FINE, "Scanner content[%02d/%02d]: %s", sc->read, sc->length, sc->str);

    network_command_prepare(p->p_cmd_out, NCT_LEAD_POLO);
    //    network_command_append_long(p->p_cmd_out, their_stamp);

    netadapter_send_command(p->p_netadapter, p_cli->connection, p->p_cmd_out);

    return 0;
}

int _exe_solo_lead_polo ENGINE_HANDLE_FUNC_HEADER{
    long my_timestamp = strsc_long(sc);
    long now = (unsigned long) time(NULL);

    p_cli->latency = now - my_timestamp;

    glog(LOG_FINE, "Latency with client %s is %d", p_cli->name, p_cli->latency);

    game_room *p_gr = engine_room_by_client(p, p_cli);
    if (p_gr) {
        p->p_cmd_out->type = NCT_ROOM_CLIENT_LATENCY;
        network_command_append_short(p->p_cmd_out, p_cli->latency);

        netadapter_broadcast_command_p(p->p_netadapter, p_gr->clients, p_gr->size, p->p_cmd_out);
    }

    return 0;
}

int _exe_solo_client_set_name ENGINE_HANDLE_FUNC_HEADER{
    net_client_set_name(p_cli, sc->str, sc->length);

    p->p_cmd_out->type = NCT_CLIENT_SET_NAME;
    network_command_append_str(p->p_cmd_out, p_cli->name, strlen(p_cli->name));

    engine_send_command(p, p_cli, p->p_cmd_out);

    return 0;
}

int _exe_solo_rooms_list ENGINE_HANDLE_FUNC_HEADER{
    int i;
    my_byte n = 0;
    game_room *p_gr;
    p->p_cmd_out->type = NCT_ROOMS_LIST;
    network_command_append_byte(p->p_cmd_out, 0);

    glog(LOG_FINE, "Game room count is %d (%X)", n, n);

    for (i = 0; i < p->resources->game_rooms_length; i++) {
        if (!network_command_has_room_for(p->p_cmd_out, NETWORK_COMMAND_GAME_ROOM_LENGTH)) {
            write_hex_byte(p->p_cmd_out->data, n);
            netadapter_send_command(p->p_netadapter, p_cli->connection, p->p_cmd_out);

            n = 0;
            network_command_prepare(p->p_cmd_out, NCT_ROOMS_LIST);
            network_command_append_byte(p->p_cmd_out, 0);
        }

        p_gr = p->resources->game_rooms + i;
        if (p_gr->game_state != GAME_ROOM_STATE_DONE && p_gr->game_state != GAME_ROOM_STATE_IDLE) {
            n++;
            network_command_append_short(p->p_cmd_out, i);
            network_command_append_byte(p->p_cmd_out, p_gr->size);
            network_command_append_byte(p->p_cmd_out, game_room_get_open_player_slots(p_gr));
            network_command_append_byte(p->p_cmd_out, p_gr->game_state);
            network_command_append_byte(p->p_cmd_out, 0); // todo: difficulty
        }
    }

    glog(LOG_FINE, "Game room count is %d (%X)", n, n);

    write_hex_byte(p->p_cmd_out->data, n);
    engine_send_command(p, p_cli, p->p_cmd_out);



    return 0;
}

int _exe_solo_rooms_create ENGINE_HANDLE_FUNC_HEADER{
    int clientRID, i;
    game_room *p_gr;

    p_gr = engine_room_by_client(p, p_cli);
    if (p_gr != NULL) {
        glog(LOG_FINE, "Client %d tried to create room whils already being in one (%d)",
                p_cli - p->resources->clients, p_gr - p->resources->game_rooms);

        engine_put_client_into_room(p, p_cli, p_gr);
        return 0;
    }

    p_gr = engine_find_empty_game_room(p);
    if (p_gr != NULL) {
        clientRID = game_room_init(p_gr, GAME_ROOM_MAX_PLAYERS, p_cli);
        engine_put_client_into_room(p, p_cli, p_gr);

        glog(LOG_FINE, "Client %d created room %d",
                p_cli - p->resources->clients, p_gr - p->resources->game_rooms);

        return 0;
    }

    glog(LOG_FINE, "Client %d tried to create room but none was empty", p_cli - p->resources->clients);

    network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
    engine_send_command(p, p_cli, p->p_cmd_out);

    return 0;

}

int _exe_solo_rooms_join ENGINE_HANDLE_FUNC_HEADER{

}

int _exe_solo_rooms_leave ENGINE_HANDLE_FUNC_HEADER{

}

void _engine_init_solo_commands(int (**command_handle_func)ENGINE_HANDLE_FUNC_HEADER) {
    command_handle_func[NCT_UNDEFINED] = &_exe_solo_undefined;
    command_handle_func[NCT_LEAD_DISCONNECT] = &_exe_solo_lead_disconnect;
    //    command_handle_func[NCT_LEAD_MARCO] = &_exe_solo_lead_marco;
    //    command_handle_func[NCT_LEAD_POLO] = &_exe_solo_lead_polo;
    command_handle_func[NCT_CLIENT_SET_NAME] = &_exe_solo_client_set_name;
    command_handle_func[NCT_ROOMS_LIST] = &_exe_solo_rooms_list;
    command_handle_func[NCT_ROOMS_CREATE] = &_exe_solo_rooms_create;
    command_handle_func[NCT_ROOMS_JOIN] = &_exe_solo_rooms_join;
    command_handle_func[NCT_ROOMS_LEAVE] = &_exe_solo_rooms_leave;
}