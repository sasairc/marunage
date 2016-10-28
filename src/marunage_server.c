/*
 * marunage -  slack outgoing-webhooks server
 *
 * marunage_server.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>

short   do_loop = 1;

int server_main_loop(marunage_t* conf)
{
    int                 ret         = 0;

    char*               from_slack  = NULL,
        *               to_slack    = NULL;

    pid_t               pid;

    tcp_server_info_t   server;
    tcp_client_info_t   client;

    init_tcp_server(&server, SERVER_IP, conf->parg);
    if (conf->vflag == 1) {
        fprintf(stdout, "%s: Initialize TCP-Server\nServer Port = %ld\n",
                PROGNAME, conf->parg);
    }

    if (open_tcp_server(&server) < 0)
        return -1;

    /* main loop */
    while (do_loop == 1 && wait_tcp_server(&server, &client) == 0) {
        if ((pid = fork()) < 0) {
            fprintf(stderr, "%s: fork() failure\n",
                    PROGNAME);

            return -2;
        } else if (pid == 0) {
            close_tcp_server(&server);

            /* get slack query string */
            if ((ret = get_slack_post(&client, &from_slack)) < 0) {
                fprintf(stderr, "%s: get_slack_post() failure: %d",
                        PROGNAME, ret);

                exit(1);
            }

            /* query parsing after getting json */
            if ((ret = exec_parser(conf->carg, from_slack, &to_slack)) < 0) {
                fprintf(stderr, "%s: exec_parser() failure: %d\n",
                        PROGNAME, ret);

                if (from_slack != NULL) {
                    free(from_slack);
                    from_slack = NULL;
                }

                exit(2);
            } else if (ret == 1) {
                /* no sending */
                if (from_slack != NULL) {
                    free(from_slack);
                    from_slack = NULL;
                }

                exit(0);
            }

            /* send json */
            if ((ret = send_slack_post(&client, to_slack)) < 0) {
                fprintf(stderr, "%s: send_slack_post() failue: %d\n",
                        PROGNAME, ret);

                exit(3);
            }

            /* shutdown session */
            shutdown(client.data_socket, SHUT_WR);

            /* release memory */
            if (from_slack != NULL) {
                free(from_slack);
                from_slack = NULL;
            }
            if (to_slack != NULL) {
                free(to_slack);
                to_slack = NULL;
            }

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

        /* exec parser */
        execl(parser, parser, from_slack, NULL);

        perror("marunage: parser file");
        exit(1);
    } else {
        close(fd[1]);
        if ((buf = (char*)malloc(sizeof(char) * bufsiz)) == NULL) {
            fprintf(stderr, "%s: malloc() failure\n",
                    PROGNAME);

            goto ERR;
        }

        /* reading parser stdout */
        while (read(fd[0], buf + readsiz, 1) != 0) {
            if (readsiz == (bufsiz -1)) {
                bufsiz += 1024;
                if ((buf = (char*)realloc(buf, sizeof(char) * bufsiz)) == NULL) {
                    fprintf(stderr, "%s:realloc() failure\n",
                            PROGNAME);

                    goto ERR;
                }
            }
            readsiz++;
        }

        wait(&status);
        close(fd[0]);
    }

    /* no sending */
    if (memcmp(buf, "nosend", 6) == 0) {
        free(buf);
        buf = NULL;

        return 1;
    }

    *to_slack = buf;

    return 0;

ERR:
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }

    return -3;
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
