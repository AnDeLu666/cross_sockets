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

cross_socket::Buffer* ProtocolHandler(cross_socket::ConnectionsWrapper* cw, std::string conn_key, cross_socket::Buffer& buff)
{
    cross_socket::Buffer* send_buff = new cross_socket::Buffer{};
    
    
    if(!buff.data.empty())
    {
        //first request to server must be less MAX_AUTH_DATA_SIZE bytes
        std::string session_key = cw->Get_session_key(conn_key);

        if((session_key == "" && buff.data.size() <= cross_socket::MAX_AUTH_DATA_SIZE) || session_key != "")
        {

            buff.data.push_back('\0'); //string have to finish with 0
            std::string key = reinterpret_cast<const char*>(&(buff.data[0]));

            const cross_socket::byte_t* tmp_bytes;

            auto it = Methods_s.find(key);

            if(it != Methods_s.end())
            {
                switch (it->second)
                {
                    case m_auth:
                        PRINT_DBG("ProtocolHandler received auth req : %s \n", key.c_str());
                        //TODO generate key
                        session_key = "adfdsgsdg2774836422gsgdd_1";
                        tmp_bytes = reinterpret_cast<const cross_socket::byte_t*>(session_key.c_str());

                        cw->Set_session_key(conn_key, session_key);

                        send_buff->data.insert(send_buff->data.end(), tmp_bytes, tmp_bytes + session_key.size());
                    break;

                    default:
                        send_buff->data.push_back(0);
                    break;
                }
            }
        }
    }

    return send_buff;

};

int main(int argc, char const *argv[])
{

    InitMyProtocolMethods();

    //cross_socket::CrossSocketSrvTCP srv(8666); //create obj srv
    cross_socket::CrossSocketSrvUDP srv(8666); //create obj srv
    
    //set functions to deal with clients
    srv.Set_main_handler_ptr(ProtocolHandler);
    //srv.SetOptions(false);
    srv.Start(); //sart server


    while(srv._status != cross_socket::SrvStatuses::STOP)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    PRINT_DBG("the end \n");
    return 0;
}