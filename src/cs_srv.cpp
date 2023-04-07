#include "cross_socket_srv_tcp.h"
#include "cross_socket_srv_udp.h"

#include <chrono>


int main(int argc, char const *argv[])
{
    cross_socket::CrossSocketSrvTCP srv(8666);

    srv.Start();
    
    while(srv._connections.size() < 1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }


    while(srv._continue_work)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //printf("server is running \n");
    }
    
    PRINT_DBG("the end \n");
    return 0;
}