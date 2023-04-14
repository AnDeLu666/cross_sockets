#include "cross_socket.h"

namespace cross_socket
{
    Socket CrossSocket::InitNewSocket()
    {
        Socket socket_tmp = -20;

        // Creating socket file descriptor
        if((socket_tmp = socket(AF_INET,_socket_type, 0)) < 0)
        {
            perror("Socket init failure");
            _sock_error = SocketError::SOCK_INIT_ERROR;
        }
        
        return socket_tmp;
    }

    CrossSocket::CrossSocket(int socket_type, bool init_socket = true)
    : _socket_type(socket_type)
    {   
        if(_socket_type != TCP && _socket_type != UDP)
        {
            _sock_error = SocketError::SOCK_UNKNOWN_TYPE;
        }
        else
        {
            WIN_(WSADATA WSAData;) //some data
            WIN_(WSAStartup(MAKEWORD(2, 0), &WSAData);) //initialize sockets
            
            if(init_socket)
            { 
                _socket = InitNewSocket();
                PRINT_DBG("socket type : %s %d\n", (_socket_type == TCP)? "TCP" : "UDP", (int)_socket);
            }
        }
    }

    SocketError CrossSocket::GetSockError()
    {
        return _sock_error;
    }

    
    cross_socket::Buffer CrossSocket::Recv(Socket recv_sock, struct sockaddr_in* address) 
    {
        uint32_t data_size = 0; // no more then 4 bytes

        Socklen_t len = sizeof(*address);

        struct cross_socket::Buffer buff = {nullptr, 0, -1};

        // receive data size if recieved_bytes 0 - client has been disconnecter -1 error
        if(_socket_type == TCP)
        {
            buff.real_bytes = recv(recv_sock, (char *)&data_size, sizeof(uint32_t), 0);
        } 
        else if(_socket_type == UDP)
        {
            //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
            buff.real_bytes = recvfrom(recv_sock, (char *)&data_size, sizeof(uint32_t), 
                                        0 NIX_(| MSG_WAITALL), (struct sockaddr *)address, &len);
        }

        if (buff.real_bytes > 0)
        {
            PRINT_DBG("recieved bytes size %d ds %d \n", buff.real_bytes, data_size);
            
            if (data_size > 0)
            {
                buff = {new char[data_size + 1]{'\0'}, data_size};

                // recieve data 
                if(_socket_type == TCP)
                {
                    buff.real_bytes = recv(recv_sock, buff.data, data_size, 0);
                } 
                else if(_socket_type == UDP)
                {
                    //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    buff.real_bytes = recvfrom(recv_sock, buff.data, data_size, 0 NIX_(| MSG_WAITALL), 
                                                ( struct sockaddr *)address, &len);
                }

                if(buff.real_bytes > 0)
                {
                    PRINT_DBG("recieved bytes %d data size %d \n", buff.real_bytes, data_size);
                    //todo if recieved_bytes != data_size
                }
                else
                {
                    buff = {nullptr, 0};
                }

            }
        }

        return buff;
    }

    int CrossSocket::Send(Socket send_sock, struct Buffer* buff, struct sockaddr_in* address)
    {
        Socklen_t len = sizeof(*address);

        // send data size  if sent_bytes 0 - client has been disconnecter -1 error
        if(_socket_type == TCP)
        {
            buff->real_bytes = send(send_sock, (const char *)&buff->size, sizeof(uint32_t), 0);
        }
        else if(_socket_type == UDP)
        {
            //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
            buff->real_bytes = sendto(send_sock, (const char *)&buff->size, sizeof(uint32_t), 
                                 0 NIX_(| MSG_WAITALL), (const struct sockaddr *)address, len);
        }


        PRINT_DBG("sent_bytes size %d \n", buff->real_bytes);

        if (buff->real_bytes > 0)
        {
            if (buff->size >= 0)
            {
                // send data
                if(_socket_type == TCP)
                {
                    //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    buff->real_bytes = send(send_sock, buff->data, buff->size, 0);
                }
                else if(_socket_type == UDP)
                {
                    buff->real_bytes = sendto(send_sock, buff->data, buff->size, 
                                        0 NIX_(| MSG_WAITALL), (const struct sockaddr *)address, len);
                }
            }
        }

        PRINT_DBG("sent_bytes data %d \n", buff->real_bytes);

        return buff->real_bytes;
    }

    CrossSocket::~CrossSocket()
    {
        #ifdef _WIN64
            closesocket(_socket);
            WSACleanup();
        #else    
            close(_socket);  // close main(listen) socket 
            shutdown(_socket, SHUT_RDWR); 
        #endif
        PRINT_DBG("CrossSocket destr \n");
    }

} //end namespace cross_socket