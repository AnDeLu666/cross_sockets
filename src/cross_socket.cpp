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

        if(_socket_type == UDP)
        {
            SetSockOptTimeout(socket_tmp, 1); //sometimes socket hungs if do not set timeout
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
    

    void CrossSocket::SetOptions(bool send_recv_buff_size_first)
    {
        _opt._send_recv_buff_size_first = send_recv_buff_size_first;
        _opt._sock_opt = 1;
    }

    void CrossSocket::SetSockOptTimeout(Socket socket, short timeout_in_seconds)
    {
        // LINUX, MAC OS X
        NIX_(struct timeval timeout;
        timeout.tv_sec = timeout_in_seconds;
        timeout.tv_usec = 0;);

        // WINDOWS
        WIN_(DWORD timeout = timeout_in_seconds * 1000;)

        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);


    }

    SocketError CrossSocket::GetSockError()
    {
        return _sock_error;
    }

    cross_socket::Buffer& CrossSocket::Recv(Socket recv_sock, sockaddr_in* address) 
    {
        Buffer* buff = new cross_socket::Buffer{};

        Socklen_t len = sizeof(*address);

        data_size_t data_size = 0;
        long long int recv_bytes = 0;

        if(_opt._send_recv_buff_size_first) //do not receive size of next data
        {
            // receive data size if recieved_bytes 0 - client has been disconnected -1 error
            if(_socket_type == TCP)
            {
                recv_bytes = recv(recv_sock, reinterpret_cast<NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), 0);
            } 
            else if(_socket_type == UDP)
            {
                //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                recv_bytes = recvfrom(recv_sock, reinterpret_cast<NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), 
                                            0 NIX_(| MSG_WAITALL), reinterpret_cast<sockaddr*>(address), &len);
            }
            
            if(recv_bytes > 0)//test use
            {
                PRINT_DBG("recieved bytes size %lld ds %d \n", recv_bytes, data_size);
            }
        }
        
        if ((recv_bytes > 0 && data_size > 0) || (!_opt._send_recv_buff_size_first))
        {
            buff->real_bytes = 0;

            byte_t* buffer;

            data_size_t buff_size = (data_size > 0)? data_size : DEFAULT_BUFFER_SIZE;

            //todo start time after change all to async
            do
            {
                recv_bytes = 0;
                buffer = new byte_t[buff_size]{'\0'};

                // recieve data 
                if(_socket_type == TCP)
                {
                    recv_bytes = recv(recv_sock, WIN_(reinterpret_cast<char*>)(buffer), buff_size, 0);
                } 
                else if(_socket_type == UDP)
                {
                    //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    recv_bytes = recvfrom(recv_sock, WIN_(reinterpret_cast<char*>)(buffer), buff_size, 0 NIX_(| MSG_WAITALL), 
                                                reinterpret_cast<sockaddr*>(address), &len);
                }
                
                if(recv_bytes > 0 && recv_bytes <= buff_size)
                {
                    buff->data.insert(buff->data.end(), buffer, buffer + recv_bytes);
                    buff->real_bytes += recv_bytes;
                }
                else 
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds());
                }

                delete[] buffer; 

            } while ((recv_bytes == 0) || (recv_bytes == buff_size && recv_bytes != data_size) || (buff->real_bytes < data_size));
            //todo above event hungs when we really received buff_size default and it was last data
            
            if(recv_bytes > 0)  //test use
            {
                PRINT_DBG("recieved bytes %lld buffer size %d \n", buff->real_bytes, buff_size);
            }
        }

        if(recv_bytes < 0)
        {
            buff->s_r_err = recv_bytes;
        }
        
        return *buff;

    }

    int CrossSocket::Send(Socket send_sock, Buffer* buff, sockaddr_in* address)
    {
        Socklen_t len = sizeof(*address);

        long long int send_bytes;

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

            if(_opt._send_recv_buff_size_first) //do not send size of future data
            {
                // send data size  if sent_bytes 0 - client has been disconnecter -1 error
                if(_socket_type == TCP)
                {
                    send_bytes = send(send_sock, reinterpret_cast<const NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), NIX_(MSG_NOSIGNAL)WIN_(0));
                }
                else if(_socket_type == UDP)
                {
                    //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    send_bytes = sendto(send_sock, reinterpret_cast<const NIX_(byte_t)WIN_(char)*>(&data_size), sizeof(data_size_t), 
                                        0 NIX_(| MSG_WAITALL), reinterpret_cast<sockaddr*>(address), len);
                }
                
                //test use
                if(send_bytes > 0)
                {
                    PRINT_DBG("sent_bytes size %lld \n", send_bytes);
                }
                
            }

            if (send_bytes > 0 || !_opt._send_recv_buff_size_first)
            {
                if (data_size > 0) //do not send anything if data is empty
                {
                    send_bytes = 0;

                    // send data
                    if(_socket_type == TCP)
                    {
                        //winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                        send_bytes = send(send_sock, WIN_(reinterpret_cast<char*>)(&buff->data[0]), data_size, NIX_(MSG_NOSIGNAL)WIN_(0));
                    }
                    else if(_socket_type == UDP)
                    {
                        send_bytes = sendto(send_sock, WIN_(reinterpret_cast<char*>)(&buff->data[0]), data_size, 
                                            0 NIX_(| MSG_WAITALL), reinterpret_cast<sockaddr*>(address), len);
                    }
                
                    if(send_bytes > 0)
                    {
                        buff->real_bytes += send_bytes;
                    }
                    else 
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }
            }
            
            if (buff->real_bytes > 0)
            {
                PRINT_DBG("sent_bytes data %lld \n", buff->real_bytes);
            }
            else 
            {  
                buff->s_r_err = send_bytes;
            }
        }
        
        return buff->s_r_err;
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