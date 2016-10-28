/*
 * marunage -  slack outgoing-webhook wrapper
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
#include "./tcpserver.h"
#include "./signal.h"
#include "./info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>

int do_loop = 1;

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

        return -2;
    }
    if (conf.cflag == 0) {
        fprintf(stderr, "%s: --call_parser is required option\n",
                PROGNAME);

        return -3;
    }

    handl_zombie_proc();
    if (set_signal_siglist(siglist) != 0) {
        fprintf(stderr, "set_signal_siglist() failure\n");

        return 1;
    }

    server_main_loop(&conf);

    return 0;
}

int server_main_loop(marunage_t* conf)
{
    char*               from_slack  = NULL,
        *               to_slack    = NULL;

    pid_t               pid;

    tcp_server_info_t   server;
    tcp_client_info_t   client;

    init_tcp_server(&server, SERVER_IP, conf->parg);
    if (conf->vflag == 1) {
        fprintf(stdout, "%s: Initialize TCP-Server\nServer Port = %d\n",
                PROGNAME, conf->parg);
    }

    if (open_tcp_server(&server) < 0)
        return 1;

    while (do_loop == 1 && wait_tcp_server(&server, &client) == 0) {
        if ((pid = fork()) < 0) {
            fprintf(stderr, "%s: fork() failure\n",
                    PROGNAME);

            return 2;
        } else if (pid == 0) {
            close_tcp_server(&server);
            if (conf->vflag == 1) {
                fprintf(stdout, "%s: open connection\n",
                        PROGNAME);
            }
            get_slack_post(&client, &from_slack);
            if (conf->vflag == 1) {
                fprintf(stdout, "%s: from_slack = %s\n",
                        PROGNAME, from_slack);
                fprintf(stdout, "%s: exec_parser: parser = %s\n",
                        PROGNAME, conf->carg);
            }
            exec_parser(conf->carg, from_slack, &to_slack);
            if (conf->vflag == 1) {
                fprintf(stdout, "%s: to_slack = %s\n",
                        PROGNAME, to_slack);
            }
            send_slack_post(&client, to_slack);
            shutdown(client.data_socket, SHUT_WR);
            free(from_slack);
            free(to_slack);
            from_slack = NULL;
            to_slack = NULL;
            fprintf(stdout, "%s: close connection\n",
                    PROGNAME);

            exit(0);
        }
    }
    close_tcp_server(&server);
    cleaning_proc();

    return 0;
}

int get_slack_post(tcp_client_info_t* client, char** from_slack)
{
    int     x           = 0,
            c           = 0;

    short   isbody      = 1;

    char    tmp[1024]   = {'\0'},
        *   msg         = NULL,
        *   tp          = NULL;

    size_t  recvied     = 0,
            contlen     = 0;

    /*
     * get Content-Length
     */
    memset(tmp, 0, sizeof(tmp));
    while (recv(client->data_socket, &c, sizeof(char), 0) && isbody > 0) {
        switch (c) {
            case    '\n':
                tmp[x] = '\0';
                if ((tp = strstr(tmp, "Content-Length: ")) != NULL) {
                    tp += 16;
                    contlen = atoi(tp);
                    isbody = 0;
                } else {
                    memset(tmp, 0, sizeof(tmp));
                }
                x = 0;
                break;
            default:
                tmp[x] = c;
                x++;
                continue;
        }
    }

    /*
     * seek \r & \n
     */
    isbody = 0;
    do {
        recv(client->data_socket, &c, sizeof(char), 0);
        if (c == '\r') {
            recv(client->data_socket, &c, sizeof(char), 0);
            if (c == '\n') {
                isbody++;
            }
        }
    } while (isbody < 5);

    /*
     * get body
     */
    if ((msg = (char*)malloc(sizeof(char) * contlen)) == NULL) {
        fprintf(stderr, "%s, malloc() failue\n",
                PROGNAME);

        return -1;
    }
    memset(msg, 0, contlen);

    do {
        recv(client->data_socket, &c, sizeof(char), 0);
        msg[recvied] = c;
        recvied++;
    } while (recvied < contlen);

    *from_slack = msg;

    return 0;
}

int send_slack_post(tcp_client_info_t* client, char* to_slack)
{
    int     res = 0;

    size_t  bufsiz  = strlen(to_slack),
            sended  = 0;

    while (bufsiz > 0) {
        while (1) {
            res = send(client->data_socket, to_slack + sended, bufsiz, MSG_DONTWAIT);
            if (errno != EINTR)
                break;
        }
        if (res < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
            } else {
                return -1;
            }
        }
        bufsiz -= res;
        sended++;
    }
//  send(client->data_socket, to_slack, strlen(to_slack), 0);

    return 0;
}

int exec_parser(char* parser, char* from_slack, char** to_slack)
{
    int     status  = 0,
            fd[2]   = {'\0'};

    size_t  bufsiz  = 1024,
            readsiz = 0;

    char*   buf     = NULL;

    pid_t   pid;

    if (pipe(fd) < 0) {
        fprintf(stderr, "%s: pipe() failue\n",
                PROGNAME);

        return -1;
    }
    if ((pid = fork()) < 0) {
        fprintf(stderr, "%s: fork() failue\n",
                PROGNAME);

        return -2;
    } else if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execl(parser, parser, from_slack, NULL);

        perror("marunage: ");
        exit(1);
    } else {
        close(fd[1]);
        if ((buf = (char*)malloc(sizeof(char) * bufsiz)) == NULL) {
            fprintf(stderr, "%s: malloc() failure\n",
                    PROGNAME);

            return -3;
        }
        while (read(fd[0], buf + readsiz, 1) != 0) {
            if (readsiz == (bufsiz -1)) {
                bufsiz += 1024;
                if ((buf = (char*)realloc(buf, sizeof(char) * bufsiz)) == NULL) {
                    fprintf(stderr, "%s:realloc() failure\n",
                            PROGNAME);

                    return -4;
                }
            }
            readsiz++;
        }
        wait(&status);
        close(fd[0]);
    }

    *to_slack = buf;

    return 0;
}

void cleaning_proc(void)
{
    int     ret = 0;

    pid_t   pid = 0;

    do {
        pid = waitpid(-1, &ret, WNOHANG);
    } while (pid > 0);

    return;
}

void catch_signal(int sig)
{
    do_loop = 0;

    return;
}
