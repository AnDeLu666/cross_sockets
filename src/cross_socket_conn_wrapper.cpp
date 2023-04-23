#include "cross_socket_conn_wrapper.h"

namespace cross_socket
{
    void ConnectionsWrapper::AddNewConnection(std::string conn_key, Socket conn_socket)
    {
        _connections[conn_key] = std::make_shared<Connection>(conn_socket);
    }

    bool ConnectionsWrapper::Find(std::string conn_key)
    {
        return (_connections.find(conn_key) != _connections.end()) ? true : false;
    }

    ConnectionsMap::iterator ConnectionsWrapper::ItBegin()
    {
        return _connections.begin();
    }

    ConnectionsMap::iterator ConnectionsWrapper::ItEnd()
    {
        return _connections.end();
    }

    bool ConnectionsWrapper::ConnIteratorIsValid(ConnectionsMap::iterator conn_it)
    {
    return (conn_it != _connections.end()) ? true : false;
    }
    
    void ConnectionsWrapper::Set_status(std::string conn_key, ConnStatuses status)
    {
        ConnSetter<ConnStatuses>(conn_key, status, [](ConnStatuses status, ConnectionsMap::iterator conn_it){conn_it->second->_status = status;});
    }

    ConnStatuses ConnectionsWrapper::Get_status(std::string conn_key)
    {
        return ConnGetter<ConnStatuses>(conn_key, ConnStatuses::UNDEFINED, [](ConnectionsMap::iterator conn_it)
        {
            return conn_it->second->_status;
        });
    }
    
    void ConnectionsWrapper::Set_thread_ptr(std::string conn_key, std::shared_ptr<std::thread> thread_ptr)
    {
        ConnSetter<std::shared_ptr<std::thread>>(conn_key, thread_ptr, 
            [](std::shared_ptr<std::thread> thread_ptr, ConnectionsMap::iterator conn_it)
            {        
                if (conn_it->second->_thread_ptr == nullptr) //we can set it only one time
                {
                    conn_it->second->_thread_ptr = thread_ptr;
                }
            });
    }

    void ConnectionsWrapper::Set_session_key(std::string conn_key, std::string session_key)
    {
        ConnSetter<std::string>(conn_key, session_key, [](std::string session_key, ConnectionsMap::iterator conn_it)
        {
            conn_it->second->_session_key = session_key;
        });
    }
    
    std::string ConnectionsWrapper::Get_session_key(std::string conn_key)
    {
        return ConnGetter<std::string>(conn_key, "", [](ConnectionsMap::iterator conn_it)
        {
            return conn_it->second->_session_key;
        });
    }

    void ConnectionsWrapper::Set_conn_socket(std::string conn_key, Socket socket)
    {
        ConnSetter<Socket>(conn_key, socket, [](Socket socket, ConnectionsMap::iterator conn_it)
        {
            conn_it->second->_conn_socket = socket;
        });
    }

    Socket ConnectionsWrapper::Get_conn_socket(std::string conn_key)
    {
        return ConnGetter<Socket>(conn_key, -10, [](ConnectionsMap::iterator conn_it)
        {
            return conn_it->second->_conn_socket;
        });
    }

    void ConnectionsWrapper::Set_address_ptr(std::string conn_key, sockaddr_in* address_ptr)
    {
        ConnSetter<sockaddr_in*>(conn_key, address_ptr, [](sockaddr_in* address_ptr, ConnectionsMap::iterator conn_it)
        {
            conn_it->second->_address_ptr = address_ptr;;
        });
    }

    sockaddr_in* ConnectionsWrapper::Get_address_ptr(std::string conn_key)
    {
        return ConnGetter<sockaddr_in*>(conn_key, nullptr, [](ConnectionsMap::iterator conn_it)
        {
            return conn_it->second->_address_ptr;
        });
    }

    void ConnectionsWrapper::Set_send_buffer_ptr(std::string conn_key, cross_socket::Buffer* buff_ptr)
    {
        ConnSetter<cross_socket::Buffer*>(conn_key, buff_ptr, [](cross_socket::Buffer* buff_ptr, ConnectionsMap::iterator conn_it)
        {
            conn_it->second->_send_buffer_ptr = buff_ptr;;
        });
    }

    cross_socket::Buffer* ConnectionsWrapper::Get_send_buffer_ptr(std::string conn_key)
    {
        
        return ConnGetter<cross_socket::Buffer*>(conn_key, nullptr, [](ConnectionsMap::iterator conn_it)
        {
            return conn_it->second->_send_buffer_ptr;
        });
    }

    bool ConnectionsWrapper::If_send_buffer_is_nullptr(std::string conn_key)
    {
        
        return ConnGetter<bool>(conn_key, true, [](ConnectionsMap::iterator conn_it)
        {
            return (conn_it->second->_send_buffer_ptr != nullptr) ? false : true;
        });
    }

     void ConnectionsWrapper::DeleteConnection(std::string conn_key)
     {

        conn_mtx.lock();

        _connections.erase(conn_key);

        conn_mtx.unlock(); 
     }


    ConnectionsWrapper::~ConnectionsWrapper()
    {
        _connections.clear();

        PRINT_DBG("conn_wrapper_destr \n");
    }


} //end namespace cross_socket