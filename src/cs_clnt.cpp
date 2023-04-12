#include "cross_socket_clnt_tcp.h"
#include "cross_socket_clnt_udp.h"

int main(int argc, char const* argv[]) 
{   
    cross_socket::CrossSocketClntTCP clnt;

    clnt.Connect("127.0.0.1", 8666);

    while(1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
}