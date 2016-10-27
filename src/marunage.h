/*
 * marunage -  slack outgoing-webhook wrapper
 *
 * marunage.h
 *
 * Copyright (c) 2016 sasairc
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.HocevarHocevar See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */

#ifndef MARUNAGE_H
#define MARUNAGE_H

#include "./tcpserver.h"

#define MARUNAGE_ALLNO_FLAG \
    0, 0, 0, 0, 0, NULL, NULL

typedef struct {
    short   pflag;
    short   tflag;
    short   cflag;
    short   vflag;
    long    parg;
    char*   targ;
    char*   carg;
} marunage_t;

extern int server_main_loop(marunage_t* conf);
extern int get_slack_post(tcp_client_info_t* client, char** from_slack);
extern int send_slack_post(tcp_client_info_t* client, char* to_slack);
extern int exec_parser(char* parser, char* from_slack, char** to_slack);
extern void cleaning_proc(void);
extern void catch_signal(int sig);

/* MARUNAGE_H */
#endif
