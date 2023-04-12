#pragma once
#include "cross_socket_common.h"

namespace cross_socket
{   
    class Connection
    {
    private:
        void CloseSocket();

    public:
        std::thread _thread;
        Socket _conn_socket;

        Connection(Socket conn_socket);

        ~Connection();
    };

    typedef std::map<std::string, std::shared_ptr<cross_socket::Connection>> ConnectionsMap;



}//end namespace cross_socket
