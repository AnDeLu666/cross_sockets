#include "cross_socket_clnt.h"

namespace cross_socket
{
    CrossSocketClnt::CrossSocketClnt(int socket_type, bool init_socket)
    : CrossSocket(socket_type, init_socket)
    {}

    void CrossSocketClnt::Connect(std::string ip_addr_str, uint16_t port)
    {
        CrossSocketClnt::Connect(ip_addr_str, port, "");
    }

    void CrossSocketClnt::Connect(std::string ip_addr_str, uint16_t port, std::string suffix_of_conn_key)
    {
        std::string conn_key = ip_addr_str + ":" + std::to_string(port);
        if(suffix_of_conn_key != "")
        {
            conn_key += "_" + suffix_of_conn_key;
        }
        
        if(!_cw.Find(conn_key))
        {
            Socket conn_socket = InitNewSocket();

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
                    std::thread(&CrossSocketClnt::MainHandler, this, conn_key).detach();
                }
            }
        }
        
    }

    bool CrossSocketClnt::Reconnect(std::string conn_key)
    {   
        bool res = false;

        if(_cw.Find(conn_key))
        {
            _cw.CloseCurSocket(conn_key);
            
            auto conn_socket = InitNewSocket();

            if(ConnectToSocket(conn_socket, _cw.Get_address_ref(conn_key)))
            {   
                _cw.Set_conn_socket(conn_key, conn_socket);
                _cw.Set_status(conn_key, ConnStatuses::CONNECTED);
                res = true;
            }
        }

        return res;
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
        }

        return res;
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