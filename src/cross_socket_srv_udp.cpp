#include "cross_socket_srv_udp.h"


namespace cross_socket
{
    CrossSocketSrvUDP::CrossSocketSrvUDP(uint16_t port)
        : CrossSocketSrv(port), CrossSocket(UDP)
    {
    }

    void CrossSocketSrvUDP::Start_()
    {
        _address.sin_port = htons(_port);

        _srv_addr.sin_family = AF_INET; // IPv4
        _srv_addr.sin_addr.s_addr = INADDR_ANY;

        if ((_sock_error = cross_socket::Server_Bind(_socket, _port, _srv_addr)) == SocketError::EMPTY)
        {           
            _accept_thread = std::thread(&CrossSocketSrvUDP::AcceptHandler, this);
            _accept_thread.detach();

            _status = SrvStatuses::STARTED;
        }
    }


    void CrossSocketSrvUDP::AcceptHandler()
    {
        struct cross_socket::Buffer recv_buff = {nullptr, 0, -1};

        auto it = _connections.begin();

        while (_status != SrvStatuses::STOP)
        {
            recv_buff = Recv(_socket, _address);
            if (recv_buff.real_bytes <= 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                if(recv_buff.size <= MAX_AUTH_DATA_SIZE) //first request to senver must be less MAX_AUTH_DATA_SIZE bytes
                {
                    struct cross_socket::Buffer send_buff = {nullptr, 0};

                    if(_auth_handler_ptr(recv_buff))
                    {
                        //create new connection
                        uint16_t port = htons(_address.sin_port);
                        std::string conn_indx = inet_ntoa((&_address)->sin_addr);
                        conn_indx += ":" + std::to_string(port);
                    
                        if(_connections.find(conn_indx) == _connections.end())
                        {
                            _connections[conn_indx] = std::make_shared<Connection>(_socket);
                        }

                        //make new socket and bind it to new port TODO
                        
                        //start MainHandler in a new thread TODO

                        //set work ip port
                        send_buff = {(char*)conn_indx.c_str(), (uint32_t)conn_indx.size()};
                    }

                    //send data to client
                    Send(_socket, send_buff, _address);
                }
            }
        }
    }

    void CrossSocketSrvUDP::MainHandler(std::string index) //separate thread for each client
    {
        while (_status != SrvStatuses::STOP)
        {

        }
    }

    CrossSocketSrvUDP::~CrossSocketSrvUDP()
    {
        PRINT_DBG("udp class srv_destr \n");
    }

} // end namespace cross_socket