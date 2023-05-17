#include "cross_socket.h"

namespace cross_socket
{
    Socket CrossSocket::InitNewSocket()
    {
        Socket socket_tmp = -20;

        int ai_protocol = (_socket_type == UDP) ? IPPROTO_UDP : IPPROTO_IP;
        // Creating socket file descriptor
        if ((socket_tmp = socket(AF_INET, _socket_type, ai_protocol)) < 0)
        {
            perror("Socket init failure");
            _sock_error = SocketError::SOCK_INIT_ERROR;
        }

        // flag 1 turn on 0 turn off
        int flag = 1;

        if (setsockopt(socket_tmp, SOL_SOCKET, SO_REUSEADDR NIX_(| SO_REUSEPORT),
                       WIN_(reinterpret_cast<char *>)(&flag), sizeof(flag)))
        {
            perror("Setsockopt failure");
            return SocketError::SOCK_SETOPT_ERROR;
        }

        // if (_socket_type == UDP) //if socket works in blocking mode
        // {
        //     SetSockOptTimeout(_socket, DEFAULT_SOCKET_TIMEOUT); // set 1 s timeout - sometimes socket hungs
        // }

        return socket_tmp;
    }

    CrossSocket::CrossSocket(int socket_type, bool init_socket = true)
        : _socket_type(socket_type)
    {
        if (_socket_type != TCP && _socket_type != UDP)
        {
            _sock_error = SocketError::SOCK_UNKNOWN_TYPE;
        }
        else
        {
            WIN_(WSADATA WSAData;)                      // some data
            WIN_(WSAStartup(MAKEWORD(2, 0), &WSAData);) // initialize sockets

            if (init_socket)
            {
                _socket = InitNewSocket();

                PRINT_DBG("socket type : %s %d\n", (_socket_type == TCP) ? "TCP" : "UDP", (int)_socket);
            }
        }
    }

    void CrossSocket::SetSockOptTimeout(Socket socket, short recv_send_timeout_sec) // I guess it lost effect in non blocking mode TODO test
    {
        if (recv_send_timeout_sec > 0)
        {
            // LINUX, MAC OS X
            NIX_(struct timeval timeout;
                 timeout.tv_sec = recv_send_timeout_sec;
                 timeout.tv_usec = 0;);

            // WINDOWS
            WIN_(DWORD timeout = recv_send_timeout_sec * 1000;)

            setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), sizeof timeout);
        }
    }

    SocketError CrossSocket::GetSockError()
    {
        return _sock_error;
    }

    void CrossSocket::Set_received_data_handler(std::function<Buffer*(cross_socket::ConnectionsWrapper* cw, std::string conn_key, Buffer*)> func_ptr)
    {
        _received_data_handler = func_ptr;
    }

    void CrossSocket::Set_default_send_data_mode(bool send_data)
    {
        _send_data = send_data;
    }

    bool CrossSocket::ProtocolHandlerRecv(My_ssize_t recv_bytes, Buffer* buff, byte_t* buffer)
    {
        bool res = false;

        //##########KeyUDP protocol description##########
        //--------------header 8bytes--------------------
        byte_t protocol_header_size = 8; 
        
        //-----------4 bytes data size-------------------
        byte_t protocol_header_data_size = 4; 
        data_size_t user_data_size = 0;
        
        //4 bytes which keeps segment number
        byte_t protocol_header_seg_no_size = 4;
        data_size_t user_data_seg_no = 0;
        
        //------rest up to buff_size is user data--------
        
        //#########End KeyUDP protocol description########

        
        if (recv_bytes > 0)
        {
            if((_socket_type == UDP)) //todo add choosing protocol
            {
                // test_use
                // PRINT_DBG("GeT UDP PACKET with SIZE %lli\n", recv_bytes);
                // for(short x = 0; x < protocol_header_size; x++)
                //     PRINT_DBG("|%d", buffer[x]);


                for(short i = 0; i < protocol_header_data_size; i++)
                {
                    user_data_size += buffer[i] << (8 * i) ; 
                }

                PRINT_DBG("Data size ::::::::::::::::::::::::::::::::::::::::::: %d \n", user_data_size);

                byte_t last_seg_no_byte = protocol_header_data_size + protocol_header_seg_no_size;
                for(short i = protocol_header_data_size; i < last_seg_no_byte; i++)
                {
                    user_data_seg_no += buffer[i] << (8 * (i - protocol_header_data_size)) ; 
                }

                //todo add checksum
                if(user_data_size > 10000000)
                {
                    return true;
                }

                if(buff->data_size() < user_data_size) //todo fit size from time to time
                {   
                    PRINT_DBG("!!!!!!!!!!!!try to resize :::::::::::::::::::::::::::%li bytes\n", user_data_size);

                    ADD_DBG_CODE(auto begin = std::chrono::steady_clock::now();)
                    size_t tmp_sz = user_data_size + (user_data_size / DEFAULT_MSS + 1) * protocol_header_size;
                    buff->data.resize(tmp_sz, 0);

                    ADD_DBG_CODE(
                    auto end = std::chrono::steady_clock::now();
                    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
                    )

                    PRINT_DBG("!!!!!!!!!!!!resize :::::::::::::::::::::::::::%d ms\n", elapsed_ms.count());
                }
                
                PRINT_DBG("segment number : %d \n", user_data_seg_no);
                ADD_DBG_CODE(auto begin = std::chrono::steady_clock::now();)
                memcpy(&buff->data[buff->real_bytes], buffer + protocol_header_size , (recv_bytes - protocol_header_size) * sizeof(byte_t));
                //buff->data.insert(buff->data.end(), buffer + protocol_header_size, buffer + recv_bytes);
                ADD_DBG_CODE(
                auto end = std::chrono::steady_clock::now();
                auto elapsed_micros = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
                )

                ADD_DBG_CODE(if(elapsed_micros.count() > 1) )
                    PRINT_DBG("!!!!!!!!!!!!insert ::udp:::::::::bytes::::::::%lli time %d microsec\n", recv_bytes, elapsed_micros.count());
                buff->real_bytes += recv_bytes - protocol_header_size;
                
                if(buff->real_bytes >= user_data_size)
                {
                    res = true;
                }          
            }
            else
            {
                PRINT_DBG("Dbg TCP :::::::::::::::::::::::::::::::::uder data: %d \n", user_data_size);
                ADD_DBG_CODE(auto begin = std::chrono::steady_clock::now();)
                
                PRINT_DBG("Dbg TCP capacity::::%lli::: %lli \n", buff->data.capacity(),  buff->real_bytes);
                if(buff->data_size() < (buff->real_bytes + recv_bytes))
                {
                    PRINT_DBG("Dbg TCP resize:::::::::::::::::::::::::::::: %lli \n", buff->real_bytes + recv_bytes);
                    buff->data.resize((buff->real_bytes + recv_bytes), 0);
                }

                memcpy(&buff->data[buff->real_bytes], buffer, recv_bytes * sizeof(byte_t));
                //buff->data.insert(buff->data.end(), buffer, buffer + recv_bytes);
                
                ADD_DBG_CODE(
                auto end = std::chrono::steady_clock::now();
                auto elapsed_micros = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
                )

                ADD_DBG_CODE(if(elapsed_micros.count() > 1) )
                    PRINT_DBG("!!!!!!!!!!!!insert :tcp::::::bytes::::::::%lli time %d microsec\n", recv_bytes, elapsed_micros.count());
                buff->real_bytes += recv_bytes;
            }
        }
        else if(recv_bytes == 0)
        {
            if(user_data_size == 0 || (user_data_size > 0 && buff->real_bytes >= user_data_size))
            {
                res = true;
            }            
        }
        else
        {
            res = true;
        }


        return res;
    }


    cross_socket::Buffer* CrossSocket::Recv(Socket recv_sock, byte_t* segment_buffer, Buffer* recv_buff)
    {
        //tcp bind to socket
        sockaddr_in address{};
        return Recv(recv_sock, address, segment_buffer, recv_buff);
    }

    cross_socket::Buffer* CrossSocket::Recv(Socket recv_sock, sockaddr_in &address, byte_t* segment_buffer, Buffer* recv_buff)
    {
        if (recv_sock < 0)
        {
            return nullptr;
        }

        Socklen_t len = sizeof(address);

        My_ssize_t recv_bytes = 0;

        if(recv_buff != nullptr)
        {
            recv_buff->real_bytes = 0;
        }

        data_size_t buff_size = DEFAULT_MSS; //default  segment size for all protocols
        
        bool exit = false;
        int timer = 0;

        do 
        {
            recv_bytes = 0;

            // recieve data
            if (_socket_type == TCP)
            {
                recv_bytes = recv(recv_sock, WIN_(reinterpret_cast<char *>)(segment_buffer), buff_size, 0 NIX_(| MSG_DONTWAIT ));
            }
            else if (_socket_type == UDP)
            {
                // winsocks do not support MSG_DONTWAIT MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                recv_bytes = recvfrom(recv_sock, WIN_(reinterpret_cast<char *>)(segment_buffer), buff_size, 0 NIX_(| MSG_DONTWAIT),
                                        reinterpret_cast<sockaddr *>(&address), &len);
            }

            if (recv_bytes > 0)
            {
                if(recv_buff == nullptr)
                {
                    recv_buff = new Buffer{};
                }

                timer = 0;
                PRINT_DBG("GET recieved bytes %lld buffer size %d socket : %d\n", recv_buff->real_bytes + recv_bytes, buff_size, recv_sock);
            }

            exit = ProtocolHandlerRecv(recv_bytes, recv_buff, segment_buffer);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));          


            if(timer == 500)
            {
                timer = 0;
                PRINT_DBG("GET Hungs!!!!Received is inside recv socket : %d\n", recv_sock);
            }
            timer++;

        } while (!exit);

        return recv_buff;
    }

    
    uint16_t CrossSocket::ProtocolHandlerSend(Buffer& buff, data_size_t real_data_size)
    {
        //##########KeyUDP protocol description##########
        //--------------header 8bytes--------------------
        byte_t protocol_header_size = 8; 
        
        //-----------4 bytes data size-------------------
        byte_t protocol_header_data_size = 4; 
        
        //4 bytes which keeps segment number
        byte_t protocol_header_seg_no_size = 4;
        
        //------rest up to buff_size is user data--------
        
        //#########End KeyUDP protocol description########

        data_size_t data_size = static_cast<data_size_t>(real_data_size);

        if((_socket_type == UDP)) //todo add choosing protocol
        {
            byte_t header[protocol_header_size]{0};

            data_size_t seg_no = buff.real_bytes / DEFAULT_MSS +1;
            byte_t* user_data_seg_no = reinterpret_cast<byte_t*>(&seg_no);

            byte_t* user_data_size = reinterpret_cast<byte_t*>(&real_data_size);
            for(int i = 0; i < protocol_header_data_size; i++)
            {
                header[i] = user_data_size[i];
            }

            for(int i = protocol_header_data_size; i < protocol_header_size; i++)
            {
                header[i] = user_data_seg_no[i - protocol_header_data_size];
            }
            
            //delete[] user_data_size;
            //delete[] user_data_seg_no;

            buff.data.insert(buff.data.begin() + buff.real_bytes, header, header + protocol_header_size);

            // PRINT_DBG("segment number : %d ds:\n", *user_data_seg_no, *user_data_size);
            // for(short x = 0; x < protocol_header_size; x++)
            //     PRINT_DBG("|%d", buff.data[x]);
            // PRINT_DBG("\n");


            data_size += protocol_header_size * (data_size / DEFAULT_MSS +1);
        }
        
        return (((data_size - buff.real_bytes) < DEFAULT_MSS) ? (data_size - buff.real_bytes) : DEFAULT_MSS);
                
    }
    
    int CrossSocket::Send(Socket send_sock, Buffer* buff)
    {
         //tcp bind to socket
        sockaddr_in address{};
        return Send(send_sock, buff, address);
    }

    int CrossSocket::Send(Socket send_sock, Buffer* buff, sockaddr_in &address)
    {
        if (send_sock < 0 || buff == nullptr)
        {
            return 0;
        }

        if (buff->data_size() == 0)
        {
            buff->data.emplace_back(0);
        }

        data_size_t real_data_size = static_cast<data_size_t>(buff->real_bytes);

        //buff->data.reserve((real_data_size / DEFAULT_MSS + 1)*8);

        Socklen_t len = sizeof(address);

        My_ssize_t sent_bytes = 0;
        buff->real_bytes = 0;

        if (real_data_size <= MAX_RECV_SEND_DATA_SIZE) //MAX_RECV_SEND_DATA_SIZE must  be related to data_size_t
        {
            do
            {
                sent_bytes = 0;

                // send data
                auto send_segment_size = ProtocolHandlerSend(*buff, real_data_size);
                
                if (_socket_type == TCP)
                {
                    // if do not set MSG_NOSIGNAL send will throw exception
                    sent_bytes = send(send_sock, WIN_(reinterpret_cast<char *>)(&buff->data[buff->real_bytes]),
                                        send_segment_size, NIX_(MSG_DONTWAIT | MSG_NOSIGNAL) WIN_(0));
                }
                else if (_socket_type == UDP)
                {
                    // winsocks do not support MSG_DONTWAIT MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    sent_bytes = sendto(send_sock, WIN_(reinterpret_cast<char *>)(&buff->data[buff->real_bytes]),
                                        send_segment_size, 0 NIX_(| MSG_DONTWAIT), reinterpret_cast<sockaddr *>(&address), len);
                }

                if (sent_bytes > 0)
                {
                    buff->real_bytes += sent_bytes;
                }

                PRINT_DBG("GET________________inside sent_bytes : %lli real_bytes : %lli real_data_size : %lli\n", 
                sent_bytes, buff->real_bytes, real_data_size);

            } while (sent_bytes > 0 && buff->real_bytes < real_data_size);

            // test_use
            // PRINT_DBG("list of accepted values\n");
            // PRINT_DBG("data_size : %d \n", real_data_size);
            // for(const byte_t x: buff->data)
            //    PRINT_DBG("%c", char(x));
            // PRINT_DBG("\n");
            
        }

        return buff->real_bytes;
    }


    CrossSocket::~CrossSocket()
    {
#ifdef _WIN64
        closesocket(_socket);
        WSACleanup();
#else
        close(_socket); // close main(listen) socket
        shutdown(_socket, SHUT_RDWR);
#endif
        PRINT_DBG("CrossSocket destr \n");
    }

} // end namespace cross_socket