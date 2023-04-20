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

    
    cross_socket::Buffer& CrossSocket::Recv(Socket recv_sock, sockaddr_in* address) 
    {
        return Recv(recv_sock, address, true); 
    }

    cross_socket::Buffer& CrossSocket::Recv(Socket recv_sock, sockaddr_in* address, bool buff_size_first) 
    {
        data_size_t data_size = DEFAULT_BUFFER_SIZE;

        Socklen_t len = sizeof(*address);

        Buffer* buff = new cross_socket::Buffer{};

        if(buff_size_first) //do not receive size of future data
        {
            // receive data size if recieved_bytes 0 - client has been disconnecter -1 error
            if(_socket_type == TCP)
            {
                buff->real_bytes = recv(recv_sock, reinterpret_cast<NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), 0);
            } 
            else if(_socket_type == UDP)
            {
                //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                buff->real_bytes = recvfrom(recv_sock, reinterpret_cast<NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), 
                                            0 NIX_(| MSG_WAITALL), reinterpret_cast<sockaddr*>(address), &len);
            }
        }

        if ((buff->real_bytes > 0 || !buff_size_first) && data_size > 0)
        {
            buff->real_bytes = 0;
            PRINT_DBG("recieved bytes size %lld ds %d \n", buff->real_bytes, data_size);

            byte_t* buffer;
            
            uint32_t recv_bytes = 0;

            //todo start time after change all to async
            while (buff->real_bytes < data_size)
            {
                buffer = new byte_t[data_size]{'\0'};
                // recieve data 
                if(_socket_type == TCP)
                {
                    recv_bytes = recv(recv_sock, WIN_(reinterpret_cast<char*>)(buffer), data_size, 0);
                } 
                else if(_socket_type == UDP)
                {
                    //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    recv_bytes = recvfrom(recv_sock, WIN_(reinterpret_cast<char*>)(buffer), data_size, 0 NIX_(| MSG_WAITALL), 
                                                reinterpret_cast<sockaddr*>(address), &len);
                }
                
                if(recv_bytes > 0 && recv_bytes <= data_size)
                {
                    buff->data.insert(buff->data.end(), buffer, buffer + recv_bytes);
                    buff->real_bytes += recv_bytes;
                    
                    delete[] buffer;
                } 
                else 
                {
                    //todo add logic how to deal if recv_bytes > 0 but < received data_size
                    delete[] buffer;

                    break;
                }

            }

            if(buff->real_bytes > 0)
            {
                PRINT_DBG("recieved bytes %lld data size %d \n", buff->real_bytes, data_size);
                //todo if recieved_bytes != data_size
            }
            else
            {
                *buff = {};
            }
        }

        return *buff;
    }


    int CrossSocket::Send(Socket send_sock, Buffer* buff, sockaddr_in* address)
    {
        return Send(send_sock, buff, address, true);
    }

    int CrossSocket::Send(Socket send_sock, Buffer* buff, sockaddr_in* address, bool buff_size_first)
    {
        Socklen_t len = sizeof(*address);

        buff->real_bytes = 0;

        if(buff->data.size() <= MAX_RECV_SEND_DATA_SIZE)
        {   
            data_size_t data_size = buff->data.size();

            //// test_use
            // PRINT_DBG("list of accepted values\n");
            // PRINT_DBG("data_size : %d \n", data_size);
            // PRINT_DBG("buff->data : %s \n", reinterpret_cast<const char*>(&buff->data[0]));
            // for(const byte_t x: buff->data)
            //     PRINT_DBG("%c", char(x));
            // PRINT_DBG("\n");

            if(buff_size_first) //do not send size of future data
            {
                // send data size  if sent_bytes 0 - client has been disconnecter -1 error
                if(_socket_type == TCP)
                {
                    buff->real_bytes = send(send_sock, reinterpret_cast<const NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), 0);
                }
                else if(_socket_type == UDP)
                {
                    //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    buff->real_bytes = sendto(send_sock, reinterpret_cast<const NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), 
                                        0 NIX_(| MSG_WAITALL), reinterpret_cast<sockaddr*>(address), len);
                }
            }


            PRINT_DBG("sent_bytes size %lld \n", buff->real_bytes);

            if (buff->real_bytes > 0 || !buff_size_first)
            {
                if (data_size >= 0)
                {
                    // send data
                    if(_socket_type == TCP)
                    {
                        //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                        buff->real_bytes = send(send_sock, WIN_(reinterpret_cast<char*>)(&buff->data[0]), data_size, 0);
                    }
                    else if(_socket_type == UDP)
                    {
                        buff->real_bytes = sendto(send_sock, WIN_(reinterpret_cast<char*>)(&buff->data[0]), data_size, 
                                            0 NIX_(| MSG_WAITALL), reinterpret_cast<sockaddr*>(address), len);
                    }
                }
            }

            PRINT_DBG("sent_bytes data %lld \n", buff->real_bytes);
        }
        
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