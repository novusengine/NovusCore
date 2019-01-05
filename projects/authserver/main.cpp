/*
# MIT License

# Copyright(c) 2018 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/

#include "AuthSocketHandler.h"
#include "NodeSocketHandler.h"
#include <Config\ConfigHandler.h>
#include <Database\DatabaseConnector.h>

int main()
{
    if (!ConfigHandler::Setup("authserver_configuration.json"))
    {
        std::getchar();
        return 0;
    }
    DatabaseConnector::Setup("127.0.0.1", "root", "");

    asio::io_service io_service(2);
    AuthSocketHandler server(io_service, ConfigHandler::GetOption<uint16_t>("port", 3724));
    NodeSocketHandler nodeServer(io_service, ConfigHandler::GetOption<uint16_t>("nodeport", 8000));
    nodeServer.Start();
    server.Start();

    std::thread run_thread([&]
    {
        io_service.run();
    });

    std::cout << "Server Node Instance running on port: " << nodeServer.GetPort() << std::endl;
    std::cout << "Server Auth Instance running on port: " << server.GetPort() << std::endl;
    std::getchar();

    return 0;
}
