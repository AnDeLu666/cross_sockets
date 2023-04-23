#pragma once

#include "cross_socket_conn.h"

//this class is a secure wrapper to deal with connections
// connection could be delete or add anytime from anywhere
// if work with connection directly it could occurs call to undefined
//method 

namespace cross_socket
{  
    class ConnectionsWrapper
    {
    private:
        std::mutex conn_mtx;

            template<typename T>
        void ConnSetter(std::string conn_key, T new_value, std::function<void(T, ConnectionsMap::iterator)> func)
        {
            auto conn_it = _connections.find(conn_key);

            conn_mtx.lock();
            if(ConnIteratorIsValid(conn_it))
            {
                func(new_value, conn_it);
            }
            conn_mtx.unlock();
        }

        template<typename T>
        T ConnGetter(std::string conn_key, T default_value, std::function<T(ConnectionsMap::iterator)> func)
        {
            T cur_value = default_value;

            auto conn_it = _connections.find(conn_key);

            conn_mtx.lock();
            if(ConnIteratorIsValid(conn_it))
            {
                cur_value = func(conn_it);
            }
            conn_mtx.unlock();
            
            return cur_value;
        }

    protected:
        ConnectionsMap _connections; 

    public:
        ConnectionsWrapper()
        {};
        
        void AddNewConnection(std::string conn_key, Socket conn_socket);

        bool Find(std::string conn_key);

        ConnectionsMap::iterator ItBegin();
        ConnectionsMap::iterator ItEnd();

        bool ConnIteratorIsValid(ConnectionsMap::iterator conn_it);

        void Set_status(std::string conn_key, ConnStatuses status);
        ConnStatuses Get_status(std::string conn_key);

        void Set_thread_ptr(std::string conn_key, std::shared_ptr<std::thread>);

        void Set_session_key(std::string conn_key, std::string session_key);
        std::string Get_session_key(std::string conn_key);

        void Set_conn_socket(std::string conn_key, Socket);
        Socket Get_conn_socket(std::string conn_key);

        void Set_address_ptr(std::string conn_key, sockaddr_in*);
        sockaddr_in* Get_address_ptr(std::string conn_key);

        void Set_send_buffer_ptr(std::string conn_key, cross_socket::Buffer*);
        cross_socket::Buffer* Get_send_buffer_ptr(std::string conn_key);
        bool If_send_buffer_is_nullptr(std::string conn_key);

        void DeleteConnection(std::string conn_key);

        ~ConnectionsWrapper();
    };

    typedef std::map<std::string, std::shared_ptr<cross_socket::Connection>> ConnectionsMap;

}//end namespace cross_socket