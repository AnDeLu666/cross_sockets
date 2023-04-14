#include "cross_socket_clnt.h"

namespace cross_socket
{
    void CrossSocketClnt::SharedConnectHandler(ConnectionsMap& connections, std::string ip_addr_str, uint16_t port)
    {
        std::string index = ip_addr_str + ":" + std::to_string(port);
        
        if(connections.find(index) == connections.end())
        {
            Socket new_socket = GetNewSocket();

            struct sockaddr_in* server_address_ptr = new(struct sockaddr_in);
            server_address_ptr->sin_family = AF_INET; // IPv4
            server_address_ptr->sin_port = htons(port);
            // convert IPv4 address from text to binary
            server_address_ptr->sin_addr.s_addr = inet_addr(ip_addr_str.c_str()); // TODO optimize and add validation

            if(ConnectToSocket(new_socket, *server_address_ptr))
            {   
                connections[index] = std::make_shared<Connection>(new_socket);
                connections[index]->Set_address_ptr(server_address_ptr);
                connections[index]->Set_thread_ptr(std::make_shared<std::thread>(std::thread(&CrossSocketClnt::MainHandler, this, index)));
                connections[index]->Get_thread_ptr()->detach();
                connections[index]->Set_status(ConnStatuses::CONNECTED);
            }
        }
        
    }

    CrossSocketClnt::~CrossSocketClnt()
    {
        PRINT_DBG("clntd_destr \n");
    }


} //end namespace cross_socket