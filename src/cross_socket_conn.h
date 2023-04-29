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

    public:
        ConnStatuses _status = ConnStatuses::NEW;
        std::string _session_key = "";

        Socket _conn_socket;
        sockaddr_in _address{}; 

        cross_socket::Buffer* _send_buffer_ptr = nullptr;
        Connection(Socket conn_socket);

        void CloseSocket();
        ~Connection();
    };

    typedef std::map<std::string, std::unique_ptr<cross_socket::Connection>> ConnectionsMap;

}//end namespace cross_socket
