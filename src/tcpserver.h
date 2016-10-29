/*
 * marunage -  slack outgoing-webhooks server
 *
 * tcpserver.h
 *
 * Copyright (c) 2016 sasairc
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.HocevarHocevar See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_IP   0x00000000UL

typedef struct TCP_SERVER_INF_T {
    int                 wait_socket;
    struct sockaddr_in  server_addr;
} tcp_server_info_t;

typedef struct TCP_CLIENT_INF__T {
    int                 data_socket;
    struct sockaddr_in  client_addr;
} tcp_client_info_t;

extern void init_tcp_server(tcp_server_info_t* server_info, long ip, int port);
extern int open_tcp_server(tcp_server_info_t* server_info);
extern int wait_tcp_server(tcp_server_info_t* server_info, tcp_client_info_t* client_info);
extern void close_tcp_server(tcp_server_info_t* server_info);
extern void close_tcp_client(tcp_client_info_t* client_info);

/* TCPSERVER_H */
#endif
