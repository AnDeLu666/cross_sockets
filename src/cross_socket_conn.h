#pragma once
#include "cross_socket_common.h"

namespace cross_socket
{  
    enum ConnStatuses
    {
        NEW,
        CONNECTED,
        DISCONNECT
    };

    class Connection
    {
    private:
        ConnStatuses _status = ConnStatuses::NEW;
        std::shared_ptr<std::thread> _thread_ptr = nullptr;

        Socket _conn_socket;
        struct sockaddr_in* _address_ptr = nullptr; 

        cross_socket::Buffer* _send_buffer_ptr = nullptr;

        void CloseSocket();

    public:
        Connection(Socket conn_socket);

        void Set_status(ConnStatuses status);
        ConnStatuses Get_status();

        void Set_thread_ptr(std::shared_ptr<std::thread>);
        std::shared_ptr<std::thread> Get_thread_ptr();

        void Set_conn_socket(Socket);
        Socket Get_conn_socket();

        void Set_address_ptr(struct sockaddr_in*);
        struct sockaddr_in* Get_address_ptr();

        void Set_send_buffer_ptr(cross_socket::Buffer*);
        cross_socket::Buffer* Get_send_buffer_ptr();

        ~Connection();
    };

    typedef std::map<std::string, std::shared_ptr<cross_socket::Connection>> ConnectionsMap;

}//end namespace cross_socket
