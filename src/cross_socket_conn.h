#pragma once

#include "cross_socket_common.h"

namespace cross_socket
{   
    struct Buffer
    {
        char * data = nullptr;
        u_int32_t size = 0; //no more then 4 bytes
    };

    class Connection
    {
    private:
        Buffer _buffer_to;

        void CloseSocket();

    public:
        std::thread _thread;
        int _conn_socket;

        Buffer _buffer_from;

        Connection(int conn_socket);

        Buffer GetBufferFrom();
        
        Buffer* GetBufferTo();
        void SetBufferTo(char* buffer_to, u_int32_t size);

        ~Connection();
    };

    typedef std::map<std::string, std::shared_ptr<cross_socket::Connection>> ConnectionsMap;



}//end namespace cross_socket