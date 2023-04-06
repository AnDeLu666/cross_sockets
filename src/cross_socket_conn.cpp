#include "cross_socket_conn.h"

namespace cross_socket
{
    //class connection ---------------------------------------------------
    Connection::Connection(Socket conn_socket)
    : _conn_socket(conn_socket)
    {}

    void Connection::CloseSocket()
    {
    #ifdef _WIN64
        closesocket(_conn_socket);
        WSACleanup();
    #else    
        close(_conn_socket);  // close connection socket 
        shutdown(_conn_socket, SHUT_RDWR);  // close listen socket ???
    #endif
    }

    Buffer Connection::GetBufferFrom()
    {
        return _buffer_from;
    }
    
    Buffer* Connection::GetBufferTo()
    {
        return &_buffer_to;
    }

    void Connection::SetBufferTo(char* buffer_to, uint32_t size)
    {
        _buffer_to.size = size; 
        _buffer_to.data = (char*)buffer_to;
    }

    Connection::~Connection()
    {
        PRINT_DBG("conn_destr \n");

        this->CloseSocket();
    }
    //end class connection ------------------------------------------------

} //end namespace cross_socket