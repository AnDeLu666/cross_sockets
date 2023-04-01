#include "cross_socket_clnt.h"

namespace cross_socket
{

    CrossSocketClnt::CrossSocketClnt(int socket_type)
    : CrossSocket(socket_type)
    {}

    void CrossSocketClnt::ConnectionHandler(std::string index) //, char* data) 
    {
        std::string data;

        PRINT_DBG("index = %s enter command : \n", index.c_str());
        
        while(data != "exit")
        {
            std::getline(std::cin, data);

            _connections[index]->SetBufferTo((char*)data.c_str(), data.size());

            if(TCP_Send(_connections[index]) > 0)
            {
                if(TCP_Recv(_connections[index]) <= 0)
                {
                    perror("Server isn't available\n");      
                } 
            }
        }

        _continue_work = false;
    }

    void CrossSocketClnt::Connect(unsigned int port)
    {
        _address.sin_port = htons(port);

        //convert IPv4 and IPv6 addresses from text to binary 
        if(inet_pton(AF_INET, "127.0.0.1", &_address.sin_addr) < 0)
        {
            perror("Invalid address/ Address not supported\n");
            _sock_error = SocketError::INVALID_IP_ERROR;
        }

        if(connect(_socket, (struct sockaddr*)&_address, sizeof(_address)) < 0)
        {
            perror("Connection failed\n");
            _sock_error = SocketError::CONNECTION_ERROR;
        }
        else
        {   
            std::string index = std::to_string(_socket);
            _connections[index] = std::make_shared<Connection>(_socket);
            _connections[index]->_thread = std::thread(&CrossSocketClnt::ConnectionHandler, this, index);
            _connections[index]->_thread.detach();
        }
    }

    void CrossSocketClnt::Disconnect()
    {

    }

    CrossSocketClnt::~CrossSocketClnt()
    {
        ConnectionsMap::iterator it = _connections.begin();
        for( ; it != _connections.end(); it++)
        {
            PRINT_DBG("con %s \n", it->first.c_str());
        }

        PRINT_DBG("cltd_destr \n");
        this->Disconnect();
    }

} //end namespace cross_socket