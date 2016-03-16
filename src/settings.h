/* 
 * File:   settings.h
 * Author: Stepan
 *
 * Created on 29. února 2016, 13:07
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

typedef struct settings {
	const unsigned int MAX_ROOMS;
	const unsigned short MAX_PLAYERS_PER_ROOM;
	
	const int CHUNK_SIZE;
	const unsigned int MAX_TICKRATE;

	const int NETWORKS_SEND_BUFFER_SIZE;
	const int NETWORKS_RECV_BUFFER_SIZE;
	
	
	int show_summaries;
	
	int port;
	
} settings;

int settings_process_arguments(settings *p_settings, int argc, char* argv[]);




#endif	/* SETTINGS_H */

