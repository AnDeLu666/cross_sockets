#include "cross_socket_srv.h"

#include <string>
#include <chrono>


int main(int argc, char const *argv[])
{
    std::string login = "login\n";

    std::string user_pass = "root:1";

    cross_socket::CrossSocketSrv srv(8666);

    srv.Start();
    
    while(srv._connections.size() < 1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::thread t1([&srv](){
        srv.ConnectionHandler();
    });
    t1.detach();
  
    // srv._connections["0"]->thread_id.detach();


    while(srv.GetStatus() != cross_socket::STOP)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        printf("server is running \n");
    }
    
    printf("the end \n");
    return 0;
}