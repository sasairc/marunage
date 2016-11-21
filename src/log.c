/*
 * marunage -  slack outgoing-webhooks server
 *
 * log.c
 *
 * Copyright (c) 2016 sasairc
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.HocevarHocevar See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */

#include "./config.h"
#include "./marunage.h"
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int check_logfile(FILE** dest, char* path, short chkonly)
{
    FILE*   fp  = NULL;

    if ((fp = fopen(path, "a")) == NULL) {
        perror("marunage: logfile");

        return -1;
    }
    if (chkonly > 0) {
        fclose(fp);

        return 0;
    }
    *dest = fp;

    return 0;
}

int write_log(marunage_t* conf, struct sockaddr_in* client, char* body, char* ua)
{
    FILE*   fp          = NULL;

    char    tstr[29]    = {'\0'};

    time_t  t           = time(NULL);

    if (check_logfile(&fp, conf->farg, 0) < 0)
        return -1;

    strftime(tstr, sizeof(tstr), "%d/%b/%Y:%H:%M:%S %z", localtime(&t));
    fprintf(fp, "%s - - [%s] \"%s\" \"%s\"\n",
            inet_ntoa(client->sin_addr),
            tstr, body, ua);

    fclose(fp);

    return 0;
}
