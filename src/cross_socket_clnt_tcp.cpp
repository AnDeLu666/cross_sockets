#include "cross_socket_clnt_tcp.h"

namespace cross_socket
{
    CrossSocketClntTCP::CrossSocketClntTCP()
    : CrossSocketClnt(TCP, false)
    {}

    void CrossSocketClntTCP::MainHandler(std::string conn_key) 
    {
        while(_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)//TODO status == connected
        {       
            if(!_cw.If_send_buffer_is_nullptr(conn_key))
            {
                if(Send(_cw.Get_conn_socket(conn_key), _cw.Get_send_buffer_ptr(conn_key), _cw.Get_address_ptr(conn_key)) != RECV_SEND_ERR)
                {
                    auto recv_buff = Recv(_cw.Get_conn_socket(conn_key), _cw.Get_address_ptr(conn_key));
                    if(recv_buff.s_r_err == RECV_SEND_ERR)
                    {
                        perror("Server isn't available or socket problem\n");   
                        break;   
                    } 
                    else if(recv_buff.real_bytes > 0)
                    {
                        //operate received buffer in _main_handler_ptr()
                    }
                }
                else
                {
                    break;
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


    CrossSocketClntTCP::~CrossSocketClntTCP()
    {   
        PRINT_DBG("CrossSocketClntTCP destr \n");
    }

} //end namespace cross_socket