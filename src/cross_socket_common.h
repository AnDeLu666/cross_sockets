#pragma once

#ifndef _COMMON_H
#define _COMMON_H

#include <map>
#include <memory>
#include <thread>
#include <string>
#include <functional>
#include <vector>
#include <mutex>

//#define _WIN64
#ifdef _WIN64
#include <winsock2.h>
#include <ws2tcpip.h>
// #pragma comment(lib, "WS2_32.lib") //under linux make mingw it wont work
// we have to add -lwsock32 to command-line in makefile instead of
// #pragma when compiling with MinGW
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#ifdef _WIN64
typedef SOCKADDR_IN sockaddr_in; // the same name as in lunux
typedef SOCKET Socket;
typedef int Socklen_t;

#define NIX_(exp)
#define WIN_(exp) exp
#else // linux
typedef int Socket;
typedef socklen_t Socklen_t;

#define NIX_(exp) exp
#define WIN_(exp)
#endif

#define DEBUG
#ifdef DEBUG
#define PRINT_DBG(exp, ...) printf((exp), ##__VA_ARGS__)
#else
#define PRINT_DBG(exp, ...)
#endif

#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM


namespace cross_socket
{
    typedef uint8_t byte_t;
    typedef uint32_t data_size_t;

    const short RECV_SEND_ERR = -1;
    const uint16_t MAX_AUTH_DATA_SIZE = 1024; // in bytes TODO maybe limited receiving
    const uint16_t DEFAULT_BUFFER_SIZE = 1024; // bytes read write to/from network when datasize is unknown
    const data_size_t MAX_RECV_SEND_DATA_SIZE = 4294967295; //bytes

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
        SOCK_UNKNOWN_TYPE,
        MAX_AUTH_DATA_SIZE_OVRLMT
    };

    struct Buffer
    {
        std::vector<byte_t> data;
        long long int real_bytes = 0; //received or sent from/to network
        short s_r_err = 0;
    };
    

    SocketError Server_InitTCP(Socket socket, uint16_t port, int opt, struct sockaddr_in &address);
    SocketError Server_Bind(Socket socket, uint16_t port, sockaddr_in &address);

} // end namespace cross_socket

#endif