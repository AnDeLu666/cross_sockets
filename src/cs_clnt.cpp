#include "cross_socket_clnt_tcp.h"
#include "cross_socket_clnt_udp.h"

int main(int argc, char const* argv[]) 
{   
    //cross_socket::CrossSocketClnt clnt(TCP);

    //clnt.Connect(8666);

    cross_socket::CrossSocketClntTCP clnt;

    clnt.Connect("127.0.0.1", 8666);

    while(clnt._continue_work)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
}