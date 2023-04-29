#include "cross_socket_clnt_udp.h"

namespace cross_socket
{
    CrossSocketClntUDP::CrossSocketClntUDP()
    : CrossSocketClnt(UDP, false)
    {}

    void CrossSocketClntUDP::MainHandler(std::string conn_key)
    {   
        while(_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)//TODO status == connected
        {       
            if(!_cw.If_send_buffer_is_nullptr(conn_key))
            {
                if(Send(_cw.Get_conn_socket(conn_key), _cw.Get_send_buffer_ref(conn_key), _cw.Get_address_ref(conn_key)) > 0)
                {
                    auto recv_buff = Recv(_cw.Get_conn_socket(conn_key), _cw.Get_address_ref(conn_key));
                    if (GetSockoptError(_cw.Get_conn_socket(conn_key)) != SocketError::NO_ERRORS)
                    {
                        perror("Server isn't available or socket problem\n");   
                        _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);  
                    } 
                    else if(recv_buff->real_bytes > 0)
                    {
                        //operate received buffer in _main_handler_ptr()
                    }
                }
                else
                {
                    _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                }
                
                _cw.Set_send_buffer_ptr(conn_key, nullptr); //todo lost buffer
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        //starting a new thread to destroy connection
        std::thread(&CrossSocketClnt::Disconnect, this, conn_key).detach();
    }


    CrossSocketClntUDP::~CrossSocketClntUDP()
    {
        PRINT_DBG("CrossSocketClntUDP destr \n");
    }


} //end namespace cross_socket