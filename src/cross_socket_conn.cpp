#include "cross_socket_conn.h"

namespace cross_socket
{
    //class connection ---------------------------------------------------
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

    void Connection::Set_status(ConnStatuses status)
    {
        _status = status;
    }

    ConnStatuses Connection::Get_status()
    {
        return _status;
    }
    
    void Connection::Set_thread_ptr(std::shared_ptr<std::thread> thread_ptr)
    {
        _thread_ptr = thread_ptr;
    }

    std::shared_ptr<std::thread> Connection::Get_thread_ptr()
    {
        return  _thread_ptr;
    }

    void Connection::Set_conn_socket(Socket socket)
    {
        _conn_socket = socket;
    }

    Socket Connection::Get_conn_socket()
    {
        return _conn_socket;
    }

    void Connection::Set_address_ptr(struct sockaddr_in* ptr)
    {
        _address_ptr = ptr;
    }

    struct sockaddr_in* Connection::Get_address_ptr()
    {
        return _address_ptr;
    }

    void Connection::Set_send_buffer_ptr(cross_socket::Buffer* ptr)
    {
        _send_buffer_ptr = ptr;
    }

   cross_socket::Buffer* Connection::Get_send_buffer_ptr()
    {
        return _send_buffer_ptr;
    }

    Connection::~Connection()
    {
        PRINT_DBG("conn_destr \n");

        this->CloseSocket();
    }
    //end class connection ------------------------------------------------

} //end namespace cross_socket