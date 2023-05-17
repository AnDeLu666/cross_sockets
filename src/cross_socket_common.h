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
#include <cstring>

//#define _WIN64
#ifdef _WIN64
#include <winsock2.h>
#include <ws2tcpip.h>
// #pragma comment(lib, "WS2_32.lib") //under linux make mingw it wont work
// we have to add -lws2_32 to command-line in makefile instead of
// #pragma when compiling with MinGW
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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

//#define DEBUG
#ifdef DEBUG
#define PRINT_DBG(exp, ...) printf((exp), ##__VA_ARGS__)
#define ADD_DBG_CODE(exp) exp
#else
#define PRINT_DBG(exp, ...)
#define ADD_DBG_CODE(exp)
#endif


namespace cross_socket
{
    typedef uint8_t byte_t;
    typedef uint32_t data_size_t;
    typedef long long int My_ssize_t; 

    const short TCP = SOCK_STREAM;
    const short UDP = SOCK_DGRAM;

    const uint16_t MAX_AUTH_DATA_SIZE = 1024; // in bytes TODO maybe limited receiving
    const uint16_t DEFAULT_MSS = 1024; // default maximux segment size recv/send operations
    const data_size_t MAX_RECV_SEND_DATA_SIZE = 4294967295; //bytes //MAX_RECV_SEND_DATA_SIZE must  be related to data_size_t
    const uint16_t MAX_RECV_SIZE_THIS_HANDLER = 11000; //if received data size v=over it start handler in a new thread
    //const short DEFAULT_SOCKET_TIMEOUT = 1; //udp sockets use it

    enum SocketError //todo make with real codes
    {
        NO_ERRORS,
        CONNECTION_ERROR, //WSAECONNREFUSED linux - ECONNREFUSED
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
        My_ssize_t real_bytes = 0; //received or sent from/to network ssize_t in win and linux are different

        My_ssize_t data_size()
        {
            My_ssize_t value = data.size();
            return value;
        }
    };
    

    bool SetSockoptTCPKeepAlive(Socket conn_socket, int opt);
    void SetWIN_TCPNonBlockingTCPSocket(Socket socket);
    int GetSockoptError(Socket socket);

    std::string GetIP_PortStringFromAddress(sockaddr_in& address); //return ip_addr:port string

    SocketError Server_InitTCP(Socket socket, uint16_t port, struct sockaddr_in &address);
    SocketError Server_Bind(Socket socket, uint16_t port, sockaddr_in &address);

    

} // end namespace cross_socket

#endif