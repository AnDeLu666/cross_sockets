#include "cross_socket.h"

namespace cross_socket
{
        //class connections
    Connection::Connection(int conn_socket)
    : _conn_socket(conn_socket)
    {}

    int Connection::Send(const char* data) 
    {
        u_int32_t data_size = std::strlen(data); //no more then 4 bytes
        
        printf("connn -------------------- %d \n", _conn_socket);
        // send data size  if sent_bytes 0 - client has been disconnecter -1 error
        int sent_bytes = send(_conn_socket, (const char *)&data_size, sizeof(u_int32_t), 0);
        //printf("sent_bytes 1st  %d \n", sent_bytes);
        
        if(sent_bytes > 0)
        {   
            printf("get in  data_size  %d \n", data_size);
            if(data_size > 0)
            {
                // send data
                sent_bytes = send(_conn_socket, data, data_size, 0);
            }
        }
        
        printf("sent_bytes %d \n", sent_bytes);

        return sent_bytes;
    }

    int Connection::Recv()
    {
        u_int32_t data_size = 0; //no more then 4 bytes

        // receive data size if recieved_bytes 0 - client has been disconnecter -1 error
        int recieved_bytes = read(_conn_socket, (char *)&data_size, sizeof(u_int32_t));

        printf("recieved bytes data size%d ds %d \n", recieved_bytes, data_size);
        if(recieved_bytes > 0)
        {   
            if(data_size > 0)
            {
                _buffer = std::shared_ptr<char []>(new char[data_size + 1]{'\0'}, std::default_delete<char []>());
                // recieve data
                recieved_bytes = read(_conn_socket, _buffer.get(), data_size);

                printf("recieved bytes %d ds %d \n", recieved_bytes, data_size);

            }
        }

        return recieved_bytes;
    }

    void Connection::CloseSocket()
    {
    #ifdef _WIN32
        closesocket(_conn_socket);
        WSACleanup();
    #else    
        close(_conn_socket);  // close connection socket 
        shutdown(_conn_socket, SHUT_RDWR);  // close listen socket ???
    #endif
    }

    char * Connection::GetBuffer()
    {
        return _buffer.get();
    }

    Connection::~Connection()
    {
        printf("conn_destr \n");
        this->CloseSocket();
    }
    //end class connections

    CrossSocket::CrossSocket(unsigned int port)
    : _port(port)
    {   
        // Creating socket file descriptor
        if((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Socket init failure");
            _status = Status::SOCK_INIT_ERROR;
        }

        _address.sin_family = AF_INET;
    }

    Status CrossSocket::GetStatus()
    {
        return _status;
    }

    CrossSocket::~CrossSocket()
    {}
    
} //end namespace cross_socket