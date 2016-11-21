/*
 * marunage -  slack outgoing-webhooks server
 *
 * log.h
 *
 * Copyright (c) 2016 sasairc
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.HocevarHocevar See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */

#ifndef LOG_H
#define LOG_H

#include "./marunage.h"
#include <stdio.h>
#include <netinet/in.h>

extern int check_logfile(FILE** dest, char* path, short chkonly);
extern int write_log(marunage_t* conf, struct sockaddr_in* client, char* body, char* ua);

/* LOG_H */
#endif
