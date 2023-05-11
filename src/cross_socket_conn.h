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
        
        //in a common way it wont use and set if we want to implement proxy for exmpl
        //developer have to manually call send in in function which pass to client/srv
        // object with method Set_received_data_handler(your function with send call) and 
        //disable default send calls by setting it with method of client/srv object Set_default_send_data_mode(false)
        //todo close _bound_conn_key when close current  ?????
        std::string _bound_conn_key = ""; 

        Socket _conn_socket;
        sockaddr_in _address{}; 

        cross_socket::Buffer* _send_buffer_ptr = nullptr;
        Connection(Socket conn_socket);

        void CloseSocket();
        ~Connection();
    };

    typedef std::map<std::string, std::unique_ptr<cross_socket::Connection>> ConnectionsMap;

}//end namespace cross_socket
