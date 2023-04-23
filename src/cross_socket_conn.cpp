#include "cross_socket_conn.h"

namespace cross_socket
{
    Connection::Connection(Socket conn_socket)
    : _conn_socket(conn_socket)
    {}

    void Connection::CloseSocket()
    {
        if(_conn_socket >= 0 )
        {
    #ifdef _WIN64
        closesocket(_conn_socket);
        WSACleanup();
    #else    
        close(_conn_socket);  // close connection socket 
        shutdown(_conn_socket, SHUT_RDWR);  // close listen socket ???
    #endif
        }
    }

    Connection::~Connection()
    {
        _status = ConnStatuses::DISCONNECT;
        
        if(_thread_ptr->joinable())
        {
            _thread_ptr->join();
        }

        CloseSocket();

        PRINT_DBG("conn_destr \n");
    }


} //end namespace cross_socket