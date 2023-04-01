#pragma once

#include <map>
#include <memory>
#include <thread>
#include <string>
#include "cross_socket_conn.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#define DEBUG
#ifdef DEBUG
#define PRINT_DBG(exp, ...) printf((exp), ##__VA_ARGS__)
#else
#define PRINT_DBG(exp, ...)
#endif

#define TCP SOCK_STREAM
#define UDP SOCK_STREAM

namespace cross_socket
{
    enum SocketError
    {
        EMPTY,
        CONNECTION_ERROR,
        INVALID_IP_ERROR,
        SOCK_ACCEPT_ERROR,
        SOCK_BIND_ERROR,
        SOCK_INIT_ERROR,
        SOCK_LISTEN_ERROR,
        SOCK_SETOPT_ERROR,
        SOCK_UNKNOWN_TYPE
    };

    SocketError Server_Init(int socket, int port, int opt, struct sockaddr_in& address);

    int TCP_Send(std::shared_ptr<Connection> cur_conn);
    int TCP_Recv(std::shared_ptr<Connection> cur_conn);

}//end namespace cross_socket
