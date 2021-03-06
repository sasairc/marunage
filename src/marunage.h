/*
 * marunage -  slack outgoing-webhooks server
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

#define MARUNAGE_ALLNO_FLAG \
    0, 0, 0, 1, 0, NULL, NULL

typedef struct {
    short   pflag;
    short   cflag;
    short   fflag;
    short   lflag;
    long    parg;
    char*   carg;
    char*   farg;
} marunage_t;

/* MARUNAGE_H */
#endif
