/*
 * marunage -  slack outgoing-webhook wrapper
 *
 * info.c
 *
 * Copyright (c) 2016 sasairc
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.HocevarHocevar See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */

#include "./config.h"
#include "./info.h"
#include <stdio.h>
#include <stdlib.h>

int print_version(void)
{
    fprintf(stdout, "%s %d.%d.%d%s\n",
            PROGNAME, VERSION, PATCHLEVEL, SUBLEVEL, EXTRAVERSION);
            //PROGNAME, VERSION, PATCHLEVEL, SUBLEVEL, EXTRAVERSION, ARCH);

    exit(0);
}

int print_usage(void)
{
    fprintf(stdout, "\
%s %d.%d.%d%s, slack outgoing-webhook wrapper.\n\
Usage: marunage [OPTION]...\n\
\n\
Mandatory arguments to long options are mandatory for short options too.\n\
\n\
  -p,  --port=INT            recv/send port\n\
  -t,  --token=STR           slack outgoing webhook API access token\n\
  -c,  --call_parser=PATH    string parser\n\
  -q,  --quiet               quiet mode\n\
  -v,  --vervose             vervose mode\n\
\n\
       --help                display this help and exit\n\
       --version             optput version infomation and exit\n\
\n\
Report %s bugs to %s <%s>\n\
",
        PROGNAME, VERSION, PATCHLEVEL, SUBLEVEL, EXTRAVERSION,
        PROGNAME, AUTHOR, MAIL_TO);

    exit(0);
}
