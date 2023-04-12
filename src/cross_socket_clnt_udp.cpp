#include "cross_socket_clnt_udp.h"

namespace cross_socket
{
    CrossSocketClntUDP::CrossSocketClntUDP()
    : CrossSocket(UDP)
    {}

    void CrossSocketClntUDP::MainHandler(std::string index)
    {
        std::string data;

        PRINT_DBG("clnt index = %s enter command : \n", index.c_str());
        
        while(data != "exit")
        {
            std::getline(std::cin, data);

            cross_socket::Buffer send_buff = {(char*)data.c_str(), (uint32_t)data.size()};

            PRINT_DBG("send_buff : %s \n", send_buff.data);

            if(Send(_connections[index]->_conn_socket, send_buff, _address) > 0)
            {
                auto recv_buff = Recv(_socket, _address);
                if(recv_buff.real_bytes < 0)
                {
                    perror("Server isn't available\n");      
                } 

                const char *ip = inet_ntoa(_address.sin_addr);
                uint16_t port = htons(_address.sin_port);
                PRINT_DBG("received from server : %s port %d \n", ip, port);
            }
        }

    }

    void CrossSocketClntUDP::Connect(std::string ip_addr_str, uint16_t port)
    {
        _address.sin_port = htons(port);

        PRINT_DBG("connect : \n");
        //convert IPv4 and IPv6 addresses from text to binary 
        _address.sin_addr.s_addr = inet_addr(ip_addr_str.c_str()); //TODO optimize
        // if(inet_pton(AF_INET, "127.0.0.1", &_address.sin_addr.s_addr) < 0)
        // {
        //     perror("Invalid address/ Address not supported\n");
        //     _sock_error = SocketError::INVALID_IP_ERROR;
        // }
        // else
        // {   
            std::string index = std::to_string(_socket);
            _connections[index] = std::make_shared<Connection>(_socket);
            _connections[index]->_thread = std::thread(&CrossSocketClntUDP::MainHandler, this, index);
            _connections[index]->_thread.detach();
        //}
    }

    void CrossSocketClntUDP::Disconnect()
    {

    }

    CrossSocketClntUDP::~CrossSocketClntUDP()
    {
        ConnectionsMap::iterator it = _connections.begin();
        
        for( ; it != _connections.end(); it++)
        {
            PRINT_DBG("con %s \n", it->first.c_str());
        }

        PRINT_DBG("CrossSocketClntUDP destr \n");
    }


} //end namespace cross_socket