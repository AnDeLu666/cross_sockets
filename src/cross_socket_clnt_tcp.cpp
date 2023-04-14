#include "cross_socket_clnt_tcp.h"

namespace cross_socket
{
    CrossSocketClntTCP::CrossSocketClntTCP()
    : CrossSocket(TCP, false)
    {}

    void CrossSocketClntTCP::MainHandler(std::string index) //, char* data) 
    {
        if(_connections.find(index) != _connections.end())
        {
            auto conn = _connections[index];

            while(conn->Get_status() == ConnStatuses::CONNECTED)//TODO status == connected
            {       
                if(conn->Get_send_buffer_ptr() != nullptr)
                {
                    if(Send(conn->Get_conn_socket(), conn->Get_send_buffer_ptr(), conn->Get_address_ptr()) > 0)
                    {
                        auto recv_buff = Recv(conn->Get_conn_socket(), conn->Get_address_ptr());
                        if(recv_buff.real_bytes < 0) //todo think we really can receive 0 bytes
                        {
                            perror("Server isn't available or socket problem\n");      
                        } 
                        else if(recv_buff.real_bytes > 0 && recv_buff.real_bytes == static_cast<int>(recv_buff.size))
                        {
                            //operate received buffer in _main_handler_ptr()
                        }
                    }
                    
                    conn->Set_send_buffer_ptr(nullptr); //todo lost buffer
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }

    }

    bool CrossSocketClntTCP::ConnectToSocket(Socket socket, struct sockaddr_in& address)
    {
        bool res = true;
        if(connect(socket, (struct sockaddr*)&address, sizeof(address)) < 0)
        {
            perror("Connection failed\n");
            _sock_error = SocketError::CONNECTION_ERROR;
            res = false;
        }

        return res;
    }

    void CrossSocketClntTCP::Connect(std::string ip_addr_str, uint16_t port)
    {
        SharedConnectHandler(_connections, ip_addr_str, port);
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