/* 
 * File:   settings.h
 * Author: Stepan
 *
 * Created on 29. února 2016, 13:07
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

typedef struct settings {
	const int MAX_ROOMS;
	const int CHUNK_SIZE;

	int port;
} settings;

settings *settings_process_arguments(int argc, char* argv[]);




#endif	/* SETTINGS_H */

