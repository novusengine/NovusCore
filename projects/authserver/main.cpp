#include "Authsocket.h"


int main()
{
    asio::io_service io_service;
    Authsocket server(io_service);
    server.Init();

    std::thread run_thread([&]
    {
        io_service.run();
    });


    printf("Authserver Running\n\n");
    std::getchar();

    return 0;
}
