/* 
 * File:   settings.h
 * Author: Stepan
 *
 * Created on 29. února 2016, 13:07
 */

#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct settings {
    const unsigned int MAX_ROOMS;
    const unsigned short MAX_PLAYERS_PER_ROOM;

    const int CHUNK_SIZE;
    const unsigned int MAX_TICKRATE;

    const int NETWORKS_SEND_BUFFER_SIZE;
    const int NETWORKS_RECV_BUFFER_SIZE;


    char show_summaries;

    int port;

} settings;

/**
 * Parses and sets apropriate arguments to provided settings structure.
 * Arguments provided need to be already shifted not to contain filename.
 * @param p_settings settings structure to be loaded by parsed arguments
 * @param argc count of provided arguments
 * @param argv pointers to actual arguments
 * @return 0 if everything was ok, positive integer identifies the wrong argument, negative integer marks other error
 */
int settings_process_arguments(settings *p_settings, int argc, char* argv[]);




#endif /* SETTINGS_H */

