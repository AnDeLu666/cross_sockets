#include <iostream>

#include "cross_socket_clnt_tcp.h"
#include "cross_socket_clnt_udp.h"


int main(int argc, char const* argv[]) 
{   
    //cross_socket::CrossSocketClntTCP clnt;
    cross_socket::CrossSocketClntUDP clnt;

    clnt.Connect("127.0.0.1", 8666);
    //clnt.Connect("192.168.1.64", 8666);
    //for test we use 
    char in_case = 0;
    
    auto it = clnt._cw.ItBegin();

    std::string tmp_conn_key = "", conn_key = "127.0.0.1:8666";
    std::string data = "";

    while(in_case != '4')
    {
        in_case = '0';
        printf("1 - List of connections \n");
        printf("2 - Select connection \n");
        printf("3 - Send data \n");
        printf("4 - exit \n");
        std::cin >> in_case;

        switch(in_case)
        {
            case '1':
                printf("List of connections \n");
                it = clnt._cw.ItBegin();
        
                for( ; it != clnt._cw.ItEnd(); it++)
                {
                    PRINT_DBG("con %s \n", it->first.c_str());
                }

                break;

            case '2':
                printf("Enter connection index \n");
                std::cin >> tmp_conn_key;
                if(clnt._cw.Find(tmp_conn_key))
                {
                    conn_key = tmp_conn_key;
                    tmp_conn_key = "";
                }
                break;

            case '3':
                    while(data != "exit")
                    {
                        if(clnt._cw.Find(conn_key)) 
                        {
                            if(clnt._cw.If_send_buffer_is_nullptr(conn_key))
                            {
                                printf("Send data to server(type 'exit' to return previous menu): \n");
                                std::cin >>  data;

                                    auto tmp = reinterpret_cast<const cross_socket::byte_t*>(data.c_str());
                                    
                                    cross_socket::Buffer* send_buff = new cross_socket::Buffer{};
                                    send_buff->data.insert(send_buff->data.end(),tmp, tmp + data.size());

                                    clnt._cw.Set_send_buffer_ptr(conn_key, send_buff);
                            }
                        }
                        else
                        {
                            PRINT_DBG("connection is not found \n");
                            break;
                        }
                    }
                    
                    data = "";

                break;

        }
    }

    PRINT_DBG("Bye bye!!!\n");
    
    return 0;
}