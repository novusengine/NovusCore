#include "../common/TcpServer.h"

int main()
{
    // Network Thread (TEMPORARY)
    std::thread run_thread([&]
    {
        try
        {
            asio::io_service io_service;
            Common::TcpServer server(io_service);
            io_service.run();
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    });

    printf("Authserver Running\n\n");
    std::getchar();

    return 0;
}