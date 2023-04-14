#include "cross_socket_srv_tcp.h"
#include "cross_socket_srv_udp.h"
#include <chrono>


enum Methods_i //if you change names here change it everywhere
{
    m_auth = 100,
    m_send_your_ip_port
};

std::map<std::string, Methods_i> Methods_s;
    
void InitMyProtocolMethods()
{
    Methods_s.emplace("auth", m_auth);
    Methods_s.emplace("send_your_ip_port", m_send_your_ip_port);
}

bool AuthHandler(cross_socket::Buffer& buff)
{
    bool res = false;
    std::string method = buff.data;

    PRINT_DBG("AUTH --- %s \n", method.c_str());

    if(method == "auth")
    {
        PRINT_DBG("AUTH true \n");
        res = true;
    }
    
    return res;
}

cross_socket::Buffer ProtocolHandler(std::string conn_indx, cross_socket::Buffer& buff)
{
    cross_socket::Buffer send_buff = {nullptr, 0};

    std::string key = buff.data;
    auto it = Methods_s.find(key);

    if(it != Methods_s.end())
    {
        switch (it->second)
        {
            case m_auth:
            PRINT_DBG("Control protocol received data : %s \n", key.c_str());
            break;

            default:
            break;
        }
    }

    return send_buff;

};

int main(int argc, char const *argv[])
{

    InitMyProtocolMethods();

    cross_socket::CrossSocketSrvTCP srv(8666); //create obj srv
    
    //set functions to deal with clients
    srv.Set_auth_handler_ptr(AuthHandler);
    srv.Set_main_handler_ptr(ProtocolHandler);

    srv.Start(); //sart server
    
    while(srv._connections.size() < 1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }


    while(srv._status != cross_socket::SrvStatuses::STOP)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    PRINT_DBG("the end \n");
    return 0;
}