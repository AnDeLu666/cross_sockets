#include "cross_socket_clnt.h"


int main(int argc, char const* argv[]) 
{   
    cross_socket::CrossSocketClnt clnt;

    clnt.Connect(8666);

    while(clnt.GetStatus() != cross_socket::STOP)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
}