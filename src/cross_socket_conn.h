#pragma once
#include "cross_socket_common.h"

namespace cross_socket
{  
    enum ConnStatuses
    {
        UNDEFINED,
        NEW,
        CONNECTED,
        DISCONNECT
    };

    class Connection
    {
    private:
        void CloseSocket();

    public:
        ConnStatuses _status = ConnStatuses::NEW;
        std::shared_ptr<std::thread> _thread_ptr = nullptr;
        std::string _session_key = "";

        Socket _conn_socket;
        struct sockaddr_in* _address_ptr = nullptr; 

        cross_socket::Buffer* _send_buffer_ptr = nullptr;

        Connection(Socket conn_socket);

        
        ~Connection();
    };

    typedef std::map<std::string, std::shared_ptr<cross_socket::Connection>> ConnectionsMap;

}//end namespace cross_socket
