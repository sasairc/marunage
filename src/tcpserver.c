/*
 * marunage -  slack outgoing-webhooks server
 *
 * tcpserver.c
 *
 * Copyright (c) 2016 sasairc
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.HocevarHocevar See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */

#include "./tcpserver.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void init_tcp_server(tcp_server_info_t* server_info, long ip, int port)
{
    memset(server_info, 0, sizeof(*server_info));

    server_info->wait_socket = -1;

    server_info->server_addr.sin_family         = AF_INET;
    server_info->server_addr.sin_addr.s_addr    = htonl(ip);
    server_info->server_addr.sin_port           = htons(port);

    return;
}

int open_tcp_server(tcp_server_info_t* server_info)
{
    if ((server_info->wait_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");

        return -1;
    }
    if (bind(server_info->wait_socket,
            (struct sockaddr*)&server_info->server_addr,
            sizeof(server_info->server_addr)) < 0) {

        perror("bind");

        return -2;
    }

    if (listen(server_info->wait_socket, 5) < 0) {
        perror("listen");

        return -3;
    }

    return 0;
}

int wait_tcp_server(tcp_server_info_t* server_info, tcp_client_info_t* client_info)
{
    socklen_t   client_addr_len;

    memset(client_info, 0, sizeof(*client_info));
    client_info->data_socket = -1;

    while (1) {
        client_addr_len = sizeof(client_info->client_addr);
        client_info->data_socket = accept(server_info->wait_socket,
            (struct sockaddr*)&client_info->client_addr,
            &client_addr_len);

        if (client_info->data_socket != -1)
            break;

        if (errno == EINTR)
            continue;

        perror("accept");

        return -1;
    }

    return 0;
}

void close_tcp_server(tcp_server_info_t* server_info)
{
    if (server_info->wait_socket != -1) {
        close(server_info->wait_socket);
        server_info->wait_socket = -1;
    }

    return;
}

void close_tcp_client(tcp_client_info_t* client_info)
{
    if (client_info->data_socket != -1) {
        close(client_info->data_socket);
        client_info->data_socket = -1;
    }

    return;
}
