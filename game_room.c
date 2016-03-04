#include <stdlib.h>
#include <string.h>

#include "game_room.h"
#include "player.h"

game_room* game_room_create(int id, int max_players) {
	int i;

	game_room* tmp = malloc(sizeof (game_room));
	tmp->id = id;
	tmp->game_state = GAME_ROOM_STATE_LOBBY;

	tmp->max_players = max_players;

	for (i = 0; i < max_players; i++) {
		tmp->players[i] = NULL;
	}

	return tmp;
}

void game_room_delete(game_room *p_game_room) {
	free(p_game_room);
}

int game_room_has_password(game_room *p_game_room) {
	return strlen(p_game_room->password);
}

int game_room_get_open_slots(game_room *p_game_room) {
	int i, n = 0;
	
	for (i = 0; i < p_game_room->max_players; i++) {
		if (p_game_room->players[i] != NULL) {
			n++;
		}
	}

	return n;
}