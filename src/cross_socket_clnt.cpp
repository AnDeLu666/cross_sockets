#include "cross_socket_clnt.h"

namespace cross_socket
{
    CrossSocketClnt::CrossSocketClnt(int socket_type)
    : CrossSocket(socket_type, false) //false - do not arrange socket when create object
    {}

    std::string CrossSocketClnt::Connect(std::string ip_addr_str, uint16_t port)
    {
        Socket conn_socket = InitNewSocket();

        std::string conn_key = ip_addr_str + ":" + std::to_string(port) + "_" + std::to_string(conn_socket);

        struct sockaddr_in* server_address_ptr = new(struct sockaddr_in);
        server_address_ptr->sin_family = AF_INET; // IPv4
        server_address_ptr->sin_port = htons(port);
        // convert IPv4 address from text to binary
        server_address_ptr->sin_addr.s_addr = inet_addr(ip_addr_str.c_str()); // TODO optimize and add validation

        if(ConnectToSocket(conn_socket, *server_address_ptr))
        {   
            _cw.AddNewConnection(conn_key, conn_socket);

            if(_cw.Find(conn_key))
            {
                _cw.Set_address_ref(conn_key, *server_address_ptr);
                _cw.Set_status(conn_key, ConnStatuses::CONNECTED);
                std::thread(&CrossSocketClnt::SendHandler, this, conn_key).detach();
            }
        }

        return conn_key;
    }


    
    bool CrossSocketClnt::ConnectToSocket(Socket socket, struct sockaddr_in& address)
    {
        bool res = true;
        
        if(_socket_type == TCP)
        {
            if(connect(socket, (struct sockaddr*)&address, sizeof(address)) < 0)
            {
                perror("Connection failed\n");
                _sock_error = SocketError::CONNECTION_ERROR;
                res = false;
            }

            SetWIN_TCPNonBlockingTCPSocket(socket); //under linux it sets in recv send
        }

        return res;
    }

    void CrossSocketClnt::SendHandler(std::string conn_key) 
    {
        const short sleep_time_limit = 100;
        const short sleep_time_min = 10;
        short sleep_time = sleep_time_min; //milliseconds it increases if not receive anything up to sleep_time_limit

        auto conn_socket = _cw.Get_conn_socket(conn_key);
        auto conn_address = _cw.Get_address_ref(conn_key);
        
        cross_socket::Buffer* recv_buff = new Buffer{};
        byte_t* segment_buffer = new byte_t[DEFAULT_MSS]{0};

        while(_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)
        {     
            if(!_send_data || !_cw.If_send_buffer_is_nullptr(conn_key))
            {
                if(!_send_data || (Send(conn_socket, &_cw.Get_send_buffer_ref(conn_key), conn_address) > 0))
                {
                    if(_send_data)
                    {
                        _cw.Set_send_buffer_ptr(conn_key, nullptr); //free memory TODO don't delete reuse
                    }

                    do
                    {
                        //recv_buff must be initialized or use recv_buff = Recv(..., nullptr instead of recv_buff)
                        recv_buff = Recv(conn_socket, conn_address, segment_buffer, recv_buff);
                        if (GetSockoptError(conn_socket) != SocketError::NO_ERRORS)
                        { 
                            _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                            break;
                        }

                        if(recv_buff != nullptr)
                        {
                            if(recv_buff->real_bytes > 0)
                            {
                                sleep_time = sleep_time_min;
                            }
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
                        sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;

                    } while (recv_buff == nullptr || recv_buff->real_bytes <= 0);

                    if(_received_data_handler != nullptr)
                    {
                        _received_data_handler(&_cw, conn_key, recv_buff);
                    }

                    sleep_time = sleep_time_min;
                    _cw.Set_send_buffer_ptr(conn_key, nullptr); 
                    
                }
                else
                {
                    if(_send_data)
                    {
                        _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                    }
                }
            }
 
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;

        }

        delete recv_buff;
        delete[] segment_buffer;

        //starting a new thread to destroy connection
        std::thread(&CrossSocketClnt::Disconnect, this, conn_key).detach();

    }


    void CrossSocketClnt::Disconnect(std::string conn_key)
    {
        PRINT_DBG("CrossSocketClntTCP Disconnect \n");
        _cw.DeleteConnection(conn_key);
    }

    CrossSocketClnt::~CrossSocketClnt()
    {
        PRINT_DBG("clntd_destr \n");
    }


} //end namespace cross_socket