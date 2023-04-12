#include "cross_socket_clnt_tcp.h"

namespace cross_socket
{
    CrossSocketClntTCP::CrossSocketClntTCP()
    : CrossSocket(TCP)
    {}

    void CrossSocketClntTCP::MainHandler(std::string index) //, char* data) 
    {
        std::string data;

        PRINT_DBG("index = %s enter command : \n", index.c_str());
        
        while(data != "exit")
        {
            std::getline(std::cin, data);

            cross_socket::Buffer send_buff = {(char*)data.c_str(), (uint32_t)data.size()};

            if(Send(_connections[index]->_conn_socket, send_buff, _address) > 0)
            {
                auto recv_buff = Recv(_socket, _address);
                if(recv_buff.real_bytes <= 0)
                {
                    perror("Server isn't available\n");      
                } 
            }
        }

    }

    void CrossSocketClntTCP::Connect(std::string ip_addr_str, uint16_t port)
    {
        _address.sin_port = htons(port);

        //convert IPv4 and IPv6 addresses from text to binary       
        _address.sin_addr.s_addr = inet_addr(ip_addr_str.c_str());
        // if(inet_pton(AF_INET, "127.0.0.1", &_address.sin_addr) < 0)
        // {
        //     perror("Invalid address/ Address not supported\n");
        //     _sock_error = SocketError::INVALID_IP_ERROR;
        // }

        if(connect(_socket, (struct sockaddr*)&_address, sizeof(_address)) < 0)
        {
            perror("Connection failed\n");
            _sock_error = SocketError::CONNECTION_ERROR;
        }
        else
        {   
            std::string index = std::to_string(_socket);
            _connections[index] = std::make_shared<Connection>(_socket);
            _connections[index]->_thread = std::thread(&CrossSocketClntTCP::MainHandler, this, index);
            _connections[index]->_thread.detach();
        }
    }

    void CrossSocketClntTCP::Disconnect()
    {
    }

    CrossSocketClntTCP::~CrossSocketClntTCP()
    {
        ConnectionsMap::iterator it = _connections.begin();
        
        for( ; it != _connections.end(); it++)
        {
            PRINT_DBG("con %s \n", it->first.c_str());
        }
        PRINT_DBG("CrossSocketClntTCP destr \n");
        this->Disconnect();
    }

} //end namespace cross_socket