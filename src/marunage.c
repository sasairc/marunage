/*
 * marunage -  slack outgoing-webhooks server
 *
 * marunage.c
 *
 * Copyright (c) 2016 sasairc
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.HocevarHocevar See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */

#include "./config.h"
#include "./marunage.h"
#include "./marunage_server.h"
#include "./tcpserver.h"
#include "./signal.h"
#include "./info.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

int main(int argc, char* argv[])
{
    int         res     = 0,
                index   = 0;

    marunage_t  conf    = {
        MARUNAGE_ALLNO_FLAG,
    };

    siglist_t   siglist[] = {
        {SIGINT,    catch_signal},
        {SIGTERM,   catch_signal},
        {0,         NULL},
    };

    struct  option opts[] = {
        {"port",        required_argument,  NULL, 'p'},
        {"token",       required_argument,  NULL, 't'},
        {"call-parser", required_argument,  NULL, 'c'},
        {"quiet",       no_argument,        NULL, 'q'},
        {"vervose",     no_argument,        NULL, 'v'},
        {"help",        no_argument,        NULL,  0 },
        {"version",     no_argument,        NULL,  1 },
        {0, 0, 0, 0},
    };

    while ((res = getopt_long(argc, argv, ":p:t:cqv", opts, &index)) != -1) {
        switch (res) {
            case    'p':
                conf.parg = atol(optarg);
                conf.pflag = 1;
                break;
            case    't':
                conf.targ = optarg;
                conf.tflag = 1;
                break;
            case    'c':
                conf.carg = optarg;
                conf.cflag = 1;
                break;
            case    'q':
                conf.vflag = 0;
                break;
            case    'v':
                conf.vflag = 1;
                break;
            case    0:
                print_usage();
                return 0;
            case    1:
                print_version();
                return 0;
            case    '?':
                return -1;
        }
    }

    if (conf.pflag == 0) {
        conf.parg = SERVER_PORT;
    }
    if (conf.tflag == 0) {
        fprintf(stderr, "%s: --token is required option\n",
                PROGNAME);

        return 1;
    }
    if (conf.cflag == 0) {
        fprintf(stderr, "%s: --call_parser is required option\n",
                PROGNAME);

        return 2;
    }

    if (set_signal_siglist(siglist) != 0) {
        fprintf(stderr, "set_signal_siglist() failure\n");

        return 3;
    }
    handl_zombie_proc();

    if (server_main_loop(&conf) < 0)
        return 4;

    return 0;
}
